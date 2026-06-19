#pragma once 

#ifndef CORE_MEMORY_BLOCK_CPP
#define CORE_MEMORY_BLOCK_CPP

#include "core/logger/logger.hpp"
#include "core/locks/scope_lock/scope_lock.hpp"

#include "block.hpp"

#ifdef DEBUG
	static auto _core_memory_block_alloc_logger_ = CORE_GET_LOGGER(MEMORY_ALLOCATOR_LOGGER);
#else 
	static auto _core_memory_block_alloc_logger_ = nullptr;
#endif

#define _LOGGER_  _core_memory_block_alloc_logger_

namespace core {
	
/*
	constructor's
*/

core::memory_block::memory_block(u32 size , u32 max_allowed_allocations, u8 memory_tag) NOEXP {

	if (size < memory_block::min_allowed_size || size > memory_block::max_allowed_size) {
		CORE_FATAL(0, MEMORY_BLOCK_NOT_ALLOWED_SIZE, size, memory_block::min_allowed_size, size > memory_block::max_allowed_size);
		return;
	}

	u8 mem_block_tag = 0;

#ifdef DEBUG
	this->block_tag = memory_tag;
	  mem_block_tag = memory_tag;
#else
	mem_block_tag = (u8)core::allocator_tag::memory_system
#endif
	std::scoped_lock lc(this->lock);

	this->block_size = size;

	// allocate memory block
	this->start = (byte*)core::memory::allocate(
		core::g_memory_request{ 
			this->block_size, 
			mem_block_tag
		}
	);
	
	// setup variables
	this->seek = this->start;
	this->end  = this->start + this->block_size;

	// create register's
	this->active_list = core::memory_registry(max_allowed_allocations);
	this->free_list   = core::memory_registry(max_allowed_allocations);

	CORE_DEBUG(0, "new memory_block is created for {} usage , with size {}byte .", this->block_tag , this->block_size);
}

/*
	destructor
*/

memory_block::~memory_block() NOEXP {
	
	// lock memory block
	id32 id = this->lock.wait_for_lock();

	if (this->start) {
		u32 active_memory      = this->active_list.get_allocations_size();
		u32 active_allocations = this->active_list.get_allocations_count();

	#ifdef DEBUG // detected memory leaks
		if (active_memory || active_allocations) {
			CORE_WARN(CORE_LOG_CONFIG_ALL, "{} memory-leak detected with total size of {}byte !", active_allocations, active_memory);
			DEBUG_BREAK;
		}
	#endif

		// free memory block
		core::memory::deallocate(this->start);
		
		this->start = nullptr;
		this->seek  = nullptr;
		this->end   = nullptr;
	}

	CORE_DEBUG(0, "memory_block is destructed , {} usage , size {}bytes .", this->block_tag, this->block_size);

	this->lock.release(id);
}

/*
	memory_block public functions
*/

// todo: add support for memory alignement
void* core::memory_block::allocate(core::memory_request const& request) NOEXP {
	
	// if memory block is busy at the moment
	if (this->lock.is_locked()) return nullptr;

	core::atomic_scope_lock scope_lock(this->lock);

	void* pointer = nullptr;

	u32 current_free_memory = (this->seek < this->end) ? u32(this->end - this->seek) : 0u;
	
	// 1- try linear-allocation "fast"
	if (current_free_memory >= request.size) {

		// allocate from seek
		pointer = this->seek;

		// update seek
		this->seek += request.size;

		// register the allocation
		u32 result = this->active_list.insert(pointer, request.size, request.tag);

		// give back memory
		return (result < this->active_list.get_capacity()) ? pointer : nullptr;

	}

	// if active list registry is full
	if (this->active_list.get_allocations_count() >= (this->active_list.get_capacity() - 1)) {
		CORE_WARN(CORE_LOG_CONFIG_ALL, MEMORY_BLOCK_OUT_OF_MEMORY, request.size);
		return nullptr;
	}

	// 2- try to allocate from the biggest allocation in free_list "a little bit slower"
	core::i_memory_allocation allocation = this->free_list.get_biggest_allocation(request.size);
	this->handle_registry(&pointer, allocation , request);

	if (pointer) return pointer;
	
	// 3- failed to find empty place for allocation because block is either fragmeneted or full .
	// todo[IMPORTANT]: put this in different thread ---> pass it to the work_system .
	this->process_free_list();

	return nullptr;
}

void* core::memory_block::allocate(u32 size, u32 alignement, u8 tag) NOEXP {
	return this->allocate(core::memory_request{ size, alignement , tag });
}

bool core::memory_block::deallocate(void* pointer) NOEXP {
	// wait for lock
	core::atomic_scope_lock scope_lock(this->lock);

	if (pointer < this->start && pointer > this->end) {
		CORE_WARN(
			CORE_LOG_CONFIG_ALL, MEMORY_BLOCK_OUT_OF_RANGE_POINTER , 
			core::pointer_to_hex_string(pointer),
			core::pointer_to_hex_string(this->start),
			core::pointer_to_hex_string(this->end)
		);

		return false;
	}

	// cut the allocation from active list
	core::memory_allocation allocation = this->active_list.cut(pointer);

	if (allocation.ptr) {

	// put that allocation in free list
	#ifdef DEBUG
		this->free_list.insert(allocation.ptr, allocation.size, allocation.tag);
	#else
		this->free_list.insert(allocation.ptr, allocation.size, 0);
	#endif

		return true;
	}
	else {

		CORE_WARN(
			CORE_LOG_CONFIG_ALL, MEMORY_BLOCK_OUT_OF_RANGE_POINTER,
			core::pointer_to_hex_string(pointer),
			core::pointer_to_hex_string(this->start),
			core::pointer_to_hex_string(this->end)
		);

		return false;
	}

}

bool core::memory_block::is_busy() NOEXP {
	return this->lock.is_locked();
}


u32 core::memory_block::size() NOEXP {
	return this->block_size;
}

u32 core::memory_block::free_memory() NOEXP {
	// wait for lock
	core::atomic_scope_lock scope_lock(this->lock);

	return this->free_list.get_allocations_size();
}

u32 core::memory_block::allocated_memory() NOEXP {
	// wait for lock
	core::atomic_scope_lock scope_lock(this->lock);

	return this->active_list.get_allocations_size();
}


/*
	private helper functions
*/
INLINE void core::memory_block::handle_registry(
	void** ptr, core::i_memory_allocation const& allocation, core::memory_request const& request
) NOEXP {

	if (allocation.ptr) {

		// remove from free_list
		this->free_list.remove(allocation.index);

		// "allocate" by move it to active_list 
		this->active_list.insert(allocation.ptr, request.size, request.tag);

		// if memory left but it back in free_list
		if (request.size < allocation.size) {
			this->free_list.insert((byte*)allocation.ptr + request.size, allocation.size - request.size, request.tag);
		}

		*ptr = allocation.ptr;
	}
	else {
		*ptr = nullptr;
	}

}

void core::memory_block::process_free_list() NOEXP {
	// wait for lock
	core::atomic_scope_lock scope_lock(this->lock);
	
	this->free_list.merge_free_areas();
}

} // namespace core end

#endif