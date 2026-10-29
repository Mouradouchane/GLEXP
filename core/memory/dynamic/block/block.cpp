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

core::memory_block::memory_block(u64 size , u16 max_allowed_allocations, subsystem_memory_tag tag_) NOEXP {

	// check size
	if (size < min_size_allowed || size > max_size_allowed ) {
		CORE_FATAL(
			0, MEMORY_BLOCK_NOT_ALLOWED_SIZE, BYTE_TO_STRING(size) ,
			BYTE_TO_STRING(min_size_allowed), BYTE_TO_STRING(max_size_allowed)
		);
		return;
	}

	std::scoped_lock lc(_lock_);

	// setup variables
	_size_ = size;

#ifdef DEBUG
	_tag_ = tag_;
#endif

	// allocate memory
	_handle_ = core::memory::allocate(
		memory_request_g{ 
			.size = _size_,
			.tag  = _tag_
		}
	);
	
	if (_handle_.response( ) != allocator_response::success) {
		CORE_FATAL(CORE_LOG_CONFIG_ALL, ALLOCATOR_FAILED, BYTE_TO_STRING(size) , core::to_string(_tag_));
		return;
	}

	// setup variables
	_start_ = (byte*)_handle_._ptr_;
	_end_   = _start_ + _size_;

	// create register's
	_alloc_list_  = memory_registry(max_allowed_allocations);
	_free_list_   = memory_registry(max_allowed_allocations);

	// make memory free to use
	_free_list_.insert( 
		memory_allocation { 
			.ptr  = _start_ ,
			.size = _size_  ,
			.tag  = memory_tag::unkown
		}
	);

	_alive_ = true;

	CORE_DEBUG(
		0, "new memory_block is created for {} usage , {} .", 
		BYTE_TO_STRING(_size_) , core::to_string(_tag_)
	);
}

/*
	destructor
*/

core::memory_block::~memory_block() NOEXP {
	
	// lock memory block
	id32 id = _lock_.wait_for_lock();

	if (_start_) {
		u32 active_memory      = _alloc_list_.get_allocations_size();
		u32 active_allocations = _alloc_list_.get_allocations_count();

	#ifdef DEBUG // detected memory leaks
		if (active_memory || active_allocations) {
			CORE_WARN(CORE_LOG_CONFIG_ALL, "{} memory-leak detected with total size of {}byte !", active_allocations, active_memory);
			DEBUG_BREAK;
		}
	#endif

		// free memory block
		core::memory::deallocate(_handle_);
		
		_start_ = nullptr;
		_end_   = nullptr;
	}

	_alive_ = false;

	CORE_DEBUG(
		0, "memory_block is destructed , {} usage , size {}bytes .", 
		core::to_string(_tag_), BYTE_TO_STRING(_size_)
	);

	_lock_.release(id);
}


/*

	memory_block public functions

*/


memory_handle core::memory_block::allocate(memory_request const& request) NOEXP {

	return memory_block::allocate(request.size, request.alignement, request.tag, request.wait_for_memory_block);
}

memory_handle core::memory_block::allocate(u32 size_, u32 alignement_, memory_tag tag_, bool wait_for_block) NOEXP {
	DEBUG_BREAK;

	// if the block is busy at the moment
	if (_lock_.is_locked() && !wait_for_block) return MEMORY_HANDLE_BUSY;
	
	// lock the block + wait for it
	// note: becarefull you have to unlock it manualy or you get a "death-lock"
	id32 lock_id = _lock_.wait_for_lock();

	memory_handle handle;

	// check if memory_block is full
	if (size_ > _size_) {
		handle._register_index_  = -1;
		handle.response =  allocator_response::full;

		_lock_.release(lock_id);
		return handle;
	}

	// check if registry is full
	if (_alloc_list_.get_allocations_count() >= (_alloc_list_.get_capacity() - 1)) {
		CORE_WARN(CORE_LOG_CONFIG_ALL, MEMORY_BLOCK_IS_REGISTRY_FULL, _alloc_list_.get_capacity());

		handle.response = allocator_response::register_full;
		handle._register_index_ = -1;

		_lock_.release(lock_id);
		return handle;
	}

	// try to find free memory in free list , "first fit"
	memory_allocation_info allocation = _free_list_.get_allocation_with_size(size_);
	
	if (allocation.ptr) {
		// take free memory
		_free_list_.cut(allocation.index);

		// register new allocation
		handle._register_index_ = _alloc_list_.insert(allocation.ptr, size_, tag_);

		// if some free memory left
		if (size_ < allocation.size) {
			_free_list_.insert((byte*)allocation.ptr + size_, allocation.size - size_, allocation.tag);
		}

		handle.ptr = allocation.ptr;	
		handle.response = allocator_response::success;

		_lock_.release(lock_id);
		return handle;
	}
	
	/*
		failed to find free memory for allocation because block is "too fragmeneted" or "full"
	*/ 

	_lock_.release(lock_id);
	process_free_list(); // todo[IMPORTANT]: put this in different thread --> pass it to the work_system

	handle.ptr = nullptr;
	handle._register_index_ = -1;
	handle.response = allocator_response::fragmeneted;

	return handle;
}

same_pair<memory_handle> core::memory_block::allocate_tow( memory_request_2 const& request ) NOEXP {
	DEBUG_BREAK;

	// if busy + no wait
	if (_lock_.is_locked() && !request.wait_for_memory_block ) {
		return same_pair<memory_handle> { 
			MEMORY_HANDLE_BUSY,
			MEMORY_HANDLE_BUSY
		};
	}
	
	core::atomic_scope_lock scope_lock(_lock_);

	/*
		check if active registry is full
	*/ 
	if (_alloc_list_.get_allocations_count() >= (_alloc_list_.get_capacity() - 2)) {
		CORE_ERROR(CORE_LOG_CONFIG_ALL, MEMORY_BLOCK_FAILED_TO_REGISTER_ALLOCATION);

		return same_pair<memory_handle> {
			MEMORY_HANDLE_REG_FULL,
			MEMORY_HANDLE_REG_FULL
		};
	}

	/*
		try to allocate from free list
	*/ 
	memory_allocation_info allocation = _free_list_.get_allocation_with_size(request.size_1 + request.size_2);

	void* pointer_1 = nullptr;
	void* pointer_2 = nullptr;
	u32 reg_index_1 = -1;
	u32 reg_index_2 = -1;
	u32 reg_cap = _alloc_list_.get_capacity();

	handle_registry_2(&pointer_1, &pointer_2, allocation, request , reg_index_1, reg_index_2);

	if (reg_index_1 < reg_cap && reg_index_2 < reg_cap) {
		return same_pair<memory_handle> {
			memory_handle(allocator_response::success, 0, reg_index_1, pointer_1),
			memory_handle(allocator_response::success, 0, reg_index_2, pointer_2)
		};
	}
	else {
		// if one allocation failed "roll-back"
		_alloc_list_.cut(reg_index_1);
		_alloc_list_.cut(reg_index_2);
	}

	/*
		failed to find memory to allocate because block is "too fragmeneted" or "full" .
	*/ 
	
	// todo[IMPORTANT]: put this in different thread ---> pass it to the work_system .
	// note: becareful because "scope lock" my bug here !!!
	process_free_list();

	return same_pair<memory_handle> {
		MEMORY_HANDLE_FRAG,
		MEMORY_HANDLE_FRAG
	};

}

// faster
// todo: put deallocation task in sepearated thread and return immediately 
void core::memory_block::deallocate(memory_handle const& handle) NOEXP {
	DEBUG_BREAK;
	atomic_scope_lock scope_lock(_lock_);

	// get allocation
	memory_allocation allocation = _alloc_list_.cut(handle.ptr , handle._register_index_);

	// "deallocate"
	if (allocation.ptr) { 
		_free_list_.insert(allocation);
	
		CORE_DEBUG(
			0, "memory_block deallocate memory {} , size {} , used for {} .", 
			core::pointer_to_hex_string(allocation.ptr) , BYTE_TO_STRING(allocation.size) , 
			core::to_string(allocation.tag)
		);
	
		return;
	}

	// otherwise failed to deallocate
	CORE_WARN_F(
		"memory_block failed to deallocate {} passed in invalid memory_handle !", 
		core::pointer_to_hex_string(handle.ptr)
	);
}

bool core::memory_block::is_busy() NOEXP {
	return _lock_.is_locked();
}

bool core::memory_block::is_alive() NOEXP {
	return _alive_;
}

u64 core::memory_block::size() NOEXP {
	return _size_;
}

u64 core::memory_block::free_memory() NOEXP {
	core::atomic_scope_lock scope_lock(_lock_);

	return _free_list_.get_allocations_size();
}

u64 core::memory_block::allocated_memory() NOEXP {
	core::atomic_scope_lock scope_lock(_lock_);

	return _alloc_list_.get_allocations_size();
}

memory_allocation core::memory_block::get_allocation_info(memory_handle const& handle) NOEXP {

	memory_allocation info = _alloc_list_.get_allocation_info(handle._register_index_);

	if (handle.ptr == info.ptr) return info;
	
	CORE_WARN( CORE_LOG_CONFIG_ALL, 
		"invalid handle passed to memory_block.get_allocation_info function , handle not matching with what in registry !" , 
	);

	return memory_allocation{ };
}

/*
	private helper functions
*/

INLINE void core::memory_block::handle_registry_2(
	void** ptr_1,
	void** ptr_2,
	memory_allocation_info const& allocation,
	memory_request_2 const& request,
	u32& index_1,
	u32& index_2
) NOEXP {

	if (allocation.ptr) {

		// remove from free_list
		_free_list_.remove(allocation.index);

		// "allocate" by move it to active_list 
		index_1 = _alloc_list_.insert(allocation.ptr, request.size_1, request.tag_1);
		index_2 = _alloc_list_.insert((byte*)allocation.ptr + request.size_1, request.size_2, request.tag_2);

		// if memory left put it back in free_list
		if ((request.size_1 + request.size_2) < allocation.size) {
			_free_list_.insert(
				(byte*)allocation.ptr + (request.size_1 + request.size_2),
				allocation.size - (request.size_1 + request.size_2),
				memory_tag::unkown
			);
		}

		*ptr_1 = allocation.ptr;
		*ptr_2 = ((byte*)allocation.ptr + request.size_1);
	}
	else {
		*ptr_1  = nullptr;
		*ptr_2  = nullptr;
		index_1 = _alloc_list_.capacity;
		index_2 = _alloc_list_.capacity;
	}

}

void core::memory_block::process_free_list() NOEXP {
	atomic_scope_lock scope_lock(_lock_);
	
	_free_list_.merge_free_areas();
}

#endif
#endif