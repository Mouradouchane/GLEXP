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
core::memory_handle core::memory_block::allocate(core::memory_request const& request) NOEXP {

	// if the block is busy at the moment
	if (this->lock.is_locked()) {

		return core::memory_handle{
			.response = core::allocator_response::busy
		};
	}

	// lock the block
	core::atomic_scope_lock scope_lock(this->lock);

	// try allocate
	core::memory_handle handle;
	this->internal_allocate(request, handle);

	return handle;
}

core::memory_handle core::memory_block::allocate(u32 size, u32 alignement, u8 tag) NOEXP {

	// if the block is busy at the moment
	if (this->lock.is_locked()) {

		return core::memory_handle{
			.response = core::allocator_response::busy
		};
	}

	// lock the block
	core::atomic_scope_lock scope_lock(this->lock);

	// try allocate
	core::memory_handle handle;
	this->internal_allocate(core::memory_request{ size, alignement , tag }, handle);

	return handle;
}

core::memory_handle_2 core::memory_block::allocate_tow(
	core::memory_request const& request_1, core::memory_request const& request_2
) NOEXP {

	core::memory_handle handle_1;
	core::memory_handle handle_2;

	// if memory block is busy at the moment
	if (this->lock.is_locked()) {
		handle_1 = {
			.response = core::allocator_response::busy,
		};
		handle_2 = {
			.response = core::allocator_response::busy,
		};

		return core::memory_handle_2{ 
			handle_1,
			handle_2
		};
	}

	core::atomic_scope_lock scope_lock(this->lock);

	void* pointer_1 = nullptr;
	void* pointer_2 = nullptr;

	u32 current_free_memory = (this->seek < this->end) ? u32(this->end - this->seek) : 0u;

	// 1- try linear-allocation "fast"
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
		u32 result_1 = this->active_list.insert(pointer_1, request_1.size, request_1.tag);
		u32 result_2 = this->active_list.insert(pointer_2, request_2.size, request_2.tag);

	#ifdef DEBUG
		if (result_1 >= this->active_list.get_capacity()) {
			CORE_ERROR(CORE_LOG_CONFIG_ALL, "memory_block register failed to insert new registry in linear-allocation !");
		}

		if (result_2 >= this->active_list.get_capacity()) {
			CORE_ERROR(CORE_LOG_CONFIG_ALL, "memory_block register failed to insert new registry in linear-allocation !");
		}
	#endif

		// give back memory
		handle_1 = {
			.response = core::allocator_response::success,
			.ptr = pointer_1,
			.register_index = result_1
		};

		handle_2 = {
			.response = core::allocator_response::success,
			.ptr = pointer_2,
			.register_index = result_2
		};

		return core::memory_handle_2{
			handle_1 ,
			handle_2
		};
	}

	// if active list registry is full
	if (this->active_list.get_allocations_count() >= (this->active_list.get_capacity() - 2)) {
		// todo: change this error message to MEMORY_BLOCK_FAILED_TO_REGISTER_ALLOCATION
		CORE_WARN(CORE_LOG_CONFIG_ALL, MEMORY_BLOCK_OUT_OF_MEMORY, request_1.size + request_2.size);

		handle_1 = {
			.response = core::allocator_response::register_full,
		};
		handle_2 = {
			.response = core::allocator_response::register_full,
		};

		return core::memory_handle_2{
			handle_1,
			handle_2
		};
	}

	// 2- try to allocate from the biggest allocation in free_list "a little bit slower"
	core::i_memory_allocation allocation = this->free_list.get_biggest_allocation(request_1.size + request_2.size);

	u32 index_1 = 0;
	u32 index_2 = 0;

	this->handle_registry_2(&pointer_1, &pointer_2, allocation, request_1, request_2 , index_1 , index_2);

	if (pointer_1 && pointer_2) {

		handle_1 = {
			.response = core::allocator_response::success,
			.ptr = pointer_1,
			.register_index = index_1
		};
		handle_2 = {
			.response = core::allocator_response::success,
			.ptr = pointer_2,
			.register_index = index_2
		};

		return core::memory_handle_2 {
			handle_1,
			handle_2
		};
	}
	else {
		// if one allocation failed "roll-back"
		if (pointer_1) this->active_list.remove(pointer_1);
		if (pointer_2) this->active_list.remove(pointer_2);
	}

	// 3- failed to find empty place for allocation because block is either fragmeneted or full .
	// todo[IMPORTANT]: put this in different thread ---> pass it to the work_system .
	this->process_free_list();

	return core::memory_handle_2 {
			core::memory_handle{
				.response = core::allocator_response::fragmeneted 
			},
			core::memory_handle{
				.response = core::allocator_response::fragmeneted 
			}
	};
}

// faster
bool core::memory_block::deallocate(core::memory_handle handle) NOEXP {
	
	core::memory_allocation allocation = this->active_list.get_info(handle.register_index);

	if (allocation.ptr == handle.ptr) {
		this->active_list.remove(handle.register_index);
		
		this->free_list.insert(allocation.ptr, allocation.size, 0);
	
		CORE_DEBUG(0, "core::dynamic_allocator successfully deallocate : memory={} , size={} .", core::pointer_to_hex_string(allocation.ptr) , allocation.size);
		return true;
	}

	CORE_WARN_D("core::dynamic_allocator failed to deallocate : memory={} , size={} .", core::pointer_to_hex_string(allocation.ptr), allocation.size);
	return false;
}

bool core::memory_block::deallocate(void* pointer) NOEXP {
	core::atomic_scope_lock scope_lock(this->lock);

	if ((pointer < this->start) && (pointer > this->end) ) {
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
	core::atomic_scope_lock scope_lock(this->lock);

	return this->free_list.get_allocations_size();
}

u32 core::memory_block::allocated_memory() NOEXP {
	core::atomic_scope_lock scope_lock(this->lock);

	return this->active_list.get_allocations_size();
}


/*
	private helper functions
*/

INLINE u32 core::memory_block::handle_registry (
	void** ptr, core::i_memory_allocation const& allocation, core::memory_request const& request
) NOEXP {

	if (allocation.ptr) {

		// remove from free_list
		this->free_list.remove(allocation.index);

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
	core::i_memory_allocation const& allocation,
	core::memory_request const& request_1,
	core::memory_request const& request_2,
	u32& index_1,
	u32& index_2
) NOEXP {

	if (allocation.ptr) {

		// remove from free_list
		this->free_list.remove(allocation.index);

		// "allocate" by move it to active_list 
		index_1 = this->active_list.insert(allocation.ptr, request_1.size, request_1.tag);
		index_2 = this->active_list.insert((byte*)allocation.ptr + request_1.size, request_2.size, request_2.tag);

		// if memory left but it back in free_list
		if ((request_1.size + request_2.size) < allocation.size) {
			this->free_list.insert(
				(byte*)allocation.ptr + (request_1.size + request_2.size),
				allocation.size - (request_1.size + request_2.size),
				0
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

// note[WARNING]: lock the block before calling this function !
INLINE void core::memory_block::internal_allocate(
	core::memory_request const& request, core::memory_handle& handle
) NOEXP {

	void* pointer = nullptr;

	u32 current_free_memory = (this->seek < this->end) ? u32(this->end - this->seek) : 0u;

	// 1- try linear-allocation "fast"
	if (request.size <= current_free_memory) {

		// allocate from seek
		pointer = this->seek;

		// update seek
		this->seek += request.size;

		// register the allocation
		u32 result = this->active_list.insert(pointer, request.size, request.tag);

	#ifdef DEBUG
		if (result >= this->active_list.get_capacity()) {
			CORE_ERROR(CORE_LOG_CONFIG_ALL, "memory_block register failed to insert new registry in linear-allocation !");
		}
	#endif

		// give back memory
		handle.ptr = pointer;
		handle.register_index = result;
		handle.response = core::allocator_response::success;

		return;
	}

	// if active list registry is full
	if (this->active_list.get_allocations_count() >= (this->active_list.get_capacity() - 1)) {
		CORE_WARN(CORE_LOG_CONFIG_ALL, MEMORY_BLOCK_OUT_OF_MEMORY, request.size);

		handle.ptr = nullptr;
		handle.response = core::allocator_response::register_full;

		return;
	}

	// 2- try to allocate from the biggest allocation in free_list "a little bit slower"
	core::i_memory_allocation allocation = this->free_list.get_biggest_allocation(request.size);
	u32 index = this->handle_registry(&pointer, allocation, request);

	if (pointer && (index < this->active_list.capacity)) {
		handle.ptr = pointer;
		handle.register_index = index;
		handle.response = core::allocator_response::success;

		return;
	}

	// 3- failed to find empty place for allocation because block is either fragmeneted or full .
	// todo[IMPORTANT]: put this in different thread ---> pass it to the work_system .
	this->process_free_list();

	handle.ptr = nullptr;
	handle.response = core::allocator_response::fragmeneted;

	return;

}

void core::memory_block::process_free_list() NOEXP {
	core::atomic_scope_lock scope_lock(this->lock);
	
	this->free_list.merge_free_areas();
}

} // namespace core end

#endif