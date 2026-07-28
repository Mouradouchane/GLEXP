#if 1
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

/*
	constructor's
*/

core::memory_block::memory_block(u64 size , u32 max_allowed_allocations, subsystem_memory_tag tag) NOEXP {

	if (memory_block::min_allowed_size <= size <= memory_block::max_allowed_size) {
		CORE_FATAL(
			0, MEMORY_BLOCK_NOT_ALLOWED_SIZE, 
			core::bytes_to_string(size) ,
			core::bytes_to_string(memory_block::min_allowed_size), 
			core::bytes_to_string(memory_block::max_allowed_size)
		);
		return;
	}

	std::scoped_lock lc(this->lock);

#ifdef DEBUG
	this->block_tag = tag;
#endif

	this->block_size = size;

	// allocate memory block
	this->handle = core::memory::allocate(
		g_memory_request{ 
			.size = this->block_size,
			.tag  = this->block_tag
		}
	);
	
	if (this->handle.response != allocator_response::success) {
		CORE_FATAL(CORE_LOG_CONFIG_ALL, ALLOCATOR_FAILED, core::bytes_to_string(size) , core::to_string(tag));
		return;
	}

	// setup variables
	this->start = (byte*)handle.ptr;
	this->seek  = this->start;
	this->end   = this->start + this->block_size;

	// create register's
	this->active_list = core::memory_registry(max_allowed_allocations);
	this->free_list   = core::memory_registry(max_allowed_allocations);

	CORE_DEBUG(
		0, "new memory_block is created for {} usage , {} .", 
		core::bytes_to_string(this->block_size) , core::to_string(tag)
	);
}

/*
	destructor
*/

core::memory_block::~memory_block() NOEXP {
	
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
		core::memory::deallocate(this->handle);
		
		this->start = nullptr;
		this->seek  = nullptr;
		this->end   = nullptr;
	}

	CORE_DEBUG(
		0, "memory_block is destructed , {} usage , size {}bytes .", 
		core::to_string(this->block_tag), core::bytes_to_string(this->block_size)
	);

	this->lock.release(id);
}


/*
	memory_block public functions
*/


// todo: add support for memory alignement
memory_handle core::memory_block::allocate(memory_request const& request) NOEXP {

	// if the block is busy at the moment
	if (this->lock.is_locked()) {
		return memory_handle(allocator_response::busy,-1,-1,nullptr);
	}

	// lock the block
	core::atomic_scope_lock scope_lock(this->lock);

	// try allocate
	memory_handle handle;
	this->internal_allocate(request, handle);

	return handle;
}

memory_handle core::memory_block::allocate(u32 size, u32 alignement, memory_tag tag) NOEXP {

	// if the block is busy at the moment
	if (this->lock.is_locked()) {
		return memory_handle(allocator_response::busy, -1, -1, nullptr);
	}

	// lock the block
	core::atomic_scope_lock scope_lock(this->lock);

	// try allocate
	memory_handle handle;
	this->internal_allocate(
		memory_request{ .size = size, .alignement = alignement , .tag = tag }, 
		handle
	);

	return handle;
}

memory_handle_2 core::memory_block::allocate_tow(
	memory_request const& request_1, memory_request const& request_2
) NOEXP {

	memory_handle handle_1;
	memory_handle handle_2;

	// if memory block is busy at the moment
	if (this->lock.is_locked()) {
		return memory_handle_2 { 
			memory_handle(allocator_response::busy, -1, -1, nullptr),
			memory_handle(allocator_response::busy, -1, -1, nullptr)
		};
	}

	core::atomic_scope_lock scope_lock(this->lock);

	void* pointer_1 = nullptr;
	void* pointer_2 = nullptr;

	u32 current_free_memory = (this->seek < this->end) ? u32(this->end - this->seek) : 0u;

	/*
		1 - try "linear-allocation" if possible
	*/ 
	if ((request_1.size + request_2.size) <= current_free_memory) {

		// allocate 1 from seek
		pointer_1 = this->seek;

		// update seek
		this->seek += request_1.size;

		// allocate 2 from seek
		pointer_2 = this->seek;

		// update seek
		this->seek += request_2.size;

		// register the allocation
		u32 reg_index_1 = this->active_list.insert(pointer_1, request_1.size, request_1.tag);
		u32 reg_index_2 = this->active_list.insert(pointer_2, request_2.size, request_2.tag);

		// check registry insertion
		if (reg_index_1 >= this->active_list.get_capacity() || reg_index_2 >= this->active_list.get_capacity() ) {
			CORE_ERROR(
				CORE_LOG_CONFIG_ALL,"memory_block register failed to insert new registry in linear-allocation !"
			);

			// cancel allocation
			this->active_list.cut(reg_index_1);
			this->active_list.cut(reg_index_2);

			return memory_handle_2 {
				memory_handle(allocator_response::busy, -1, -1, nullptr),
				memory_handle(allocator_response::busy, -1, -1, nullptr)
			};
		}


		// give back memory
		return memory_handle_2{
			memory_handle(allocator_response::success, 0, reg_index_1, pointer_1),
			memory_handle(allocator_response::success, 0, reg_index_2, pointer_2)
		};
	}

	/*
		if active list registry is full
	*/ 
	if (this->active_list.get_allocations_count() >= (this->active_list.get_capacity() - 2)) {
		CORE_ERROR(CORE_LOG_CONFIG_ALL, MEMORY_BLOCK_FAILED_TO_REGISTER_ALLOCATION);

		return memory_handle_2 {
			memory_handle(allocator_response::register_full, -1, -1, nullptr),
			memory_handle(allocator_response::register_full, -1, -1, nullptr)
		};
	}

	/*
		2 - try to allocate from free list
	*/ 
	memory_allocation_info allocation = this->free_list.get_allocation_with_size(request_1.size + request_2.size);

	u32 reg_index_1 = -1;
	u32 reg_index_2 = -1;
	u32 reg_cap = this->active_list.get_capacity();

	this->handle_registry_2(&pointer_1, &pointer_2, allocation, request_1, request_2 , reg_index_1, reg_index_2);

	if (reg_index_1 < reg_cap && reg_index_2 < reg_cap) {
		return memory_handle_2{
			.handle_1 = memory_handle(allocator_response::success, 0, reg_index_1, pointer_1),
			.handle_2 = memory_handle(allocator_response::success, 0, reg_index_2, pointer_2)
		};
	}
	else {
		// if one allocation failed "roll-back"
		this->active_list.cut(reg_index_1);
		this->active_list.cut(reg_index_2);
	}

	/*
		3 - failed to find empty place for allocation because block is either fragmeneted or full .
	*/ 
	
	// todo[IMPORTANT]: put this in different thread ---> pass it to the work_system .
	this->process_free_list();

	return memory_handle_2 {
		.handle_1 = memory_handle(allocator_response::fragmeneted, -1, -1, nullptr),
		.handle_2 = memory_handle(allocator_response::fragmeneted, -1, -1, nullptr)
	};

}

// faster
bool core::memory_block::deallocate(memory_handle const& handle) NOEXP {
	
	memory_allocation allocation = this->active_list.cut(handle.ptr , handle._register_index_);

	if (allocation.ptr) {
		this->free_list.insert(allocation);
	
		CORE_DEBUG(
			0, "memory_block deallocate memory at {} with size {} used for {} .", 
			core::pointer_to_hex_string(allocation.ptr) , core::bytes_to_string(allocation.size) , 
			core::to_string(allocation.tag)
		);
	
		return true;
	}
	// else failed to deallocate
	CORE_WARN_D(
		"memory_block failed to deallocate memory at {} passed in memory_handle !", 
		core::pointer_to_hex_string(handle.ptr)
	);
	return false;
}


bool core::memory_block::is_busy() NOEXP {
	return this->lock.is_locked();
}


u64 core::memory_block::size() NOEXP {
	return this->block_size;
}

u64 core::memory_block::free_memory() NOEXP {
	core::atomic_scope_lock scope_lock(this->lock);

	return this->free_list.get_allocations_size();
}

u64 core::memory_block::allocated_memory() NOEXP {
	core::atomic_scope_lock scope_lock(this->lock);

	return this->active_list.get_allocations_size();
}

memory_allocation core::memory_block::get_allocation_info(memory_handle const& handle) NOEXP {

	memory_allocation info = this->active_list.get_allocation_info(handle._register_index_);

	if (handle.ptr == info.ptr) return info;
	
	CORE_WARN(
		CORE_LOG_CONFIG_ALL, 
		"invalid handle passed to memory_block.get_allocation_info function , handle not matching with what in registry !" , 
	);

	return memory_allocation{ };
}

/*
	private helper functions
*/

INLINE u32 core::memory_block::handle_registry (
	void** ptr, memory_allocation_info const& allocation, memory_request const& request
) NOEXP {

	if (allocation.ptr) {

		// remove from free_list
		this->free_list.cut(allocation.index);

		// "allocate" by move it to active_list 
		u32 index = this->active_list.insert(allocation.ptr, request.size, request.tag);

		// if memory left but it back in free_list
		if (request.size < allocation.size) {
			this->free_list.insert((byte*)allocation.ptr + request.size, allocation.size - request.size, request.tag);
		}

		*ptr = allocation.ptr;
		return index;
	}
	else {
		*ptr = nullptr;
		return this->active_list.capacity;
	}

}


INLINE void core::memory_block::handle_registry_2(
	void** ptr_1,
	void** ptr_2,
	memory_allocation_info const& allocation,
	memory_request const& request_1,
	memory_request const& request_2,
	u32& index_1,
	u32& index_2
) NOEXP {

	if (allocation.ptr) {

		// remove from free_list
		this->free_list.remove(allocation.index);

		// "allocate" by move it to active_list 
		index_1 = this->active_list.insert(allocation.ptr, request_1.size, request_1.tag);
		index_2 = this->active_list.insert((byte*)allocation.ptr + request_1.size, request_2.size, request_2.tag);

		// if memory left put it back in free_list
		if ((request_1.size + request_2.size) < allocation.size) {
			this->free_list.insert(
				(byte*)allocation.ptr + (request_1.size + request_2.size),
				allocation.size - (request_1.size + request_2.size),
				memory_tag::unkown
			);
		}

		*ptr_1 = allocation.ptr;
		*ptr_2 = ((byte*)allocation.ptr + request_1.size);
	}
	else {
		*ptr_1 = nullptr;
		*ptr_2 = nullptr;
		index_1 = this->active_list.capacity;
		index_2 = this->active_list.capacity;
	}

}

// note[WARNING]: lock the memory_block before calling this function !
INLINE void core::memory_block::internal_allocate(
	memory_request const& request, memory_handle& handle
) NOEXP {

	// check if registry is full
	if (this->active_list.get_allocations_count() >= (this->active_list.get_capacity() - 1)) {
		CORE_WARN(CORE_LOG_CONFIG_ALL, MEMORY_BLOCK_IS_REGISTRY_FULL, this->active_list.get_capacity());

		handle.ptr = nullptr;
		handle.response = allocator_response::register_full;
		handle._register_index_ = -1;

		return;
	}

	void* pointer = nullptr;
	u32 current_free_memory = (this->seek < this->end) ? u32(this->end - this->seek) : 0u;

	// 1- try linear-allocation "fast"
	if (request.size <= current_free_memory) {

		// allocate from seek
		pointer = this->seek;

		// update seek
		this->seek += request.size;

		// register the allocation
		u32 reg_index = this->active_list.insert(pointer, request.size, request.tag);

		if (reg_index >= this->active_list.get_capacity()) {
			CORE_ERROR(CORE_LOG_CONFIG_ALL, MEMORY_BLOCK_IS_REGISTRY_FULL , this->active_list.get_capacity());
			// give back memory
			handle.response = allocator_response::register_full;
			handle.ptr = nullptr;
			handle._register_index_ = -1;
		}
		else {
			// give back memory
			handle.response = allocator_response::success;
			handle.ptr = pointer;
			handle._register_index_ = reg_index;
		}

		return;
	}

	// 2- try to find empty allocation in free list , "first fit"
	memory_allocation_info allocation = this->free_list.get_allocation_with_size(request.size);
	u32 index = this->handle_registry(&pointer, allocation, request);

	if (pointer && (index < this->active_list.capacity)) {
		handle.ptr = pointer;
		handle._register_index_ = index;
		handle.response = allocator_response::success;

		return;
	}

	// 3- failed to find empty place for allocation because block is either fragmeneted or full .
	// todo[IMPORTANT]: put this in different thread ---> pass it to the work_system .
	this->process_free_list();

	handle.ptr = nullptr;
	handle.response = allocator_response::fragmeneted;

	return;

}

void core::memory_block::process_free_list() NOEXP {
	core::atomic_scope_lock scope_lock(this->lock);
	
	this->free_list.merge_free_areas();
}

#endif
#endif