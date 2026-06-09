#pragma once 

#ifndef CORE_MEMORY_BLOCK_CPP
#define CORE_MEMORY_BLOCK_CPP

#include "core/logger/logger.hpp"
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
	this->tag = memory_tag;
	mem_block_tag = memory_tag;
#else
	mem_block_tag = (u8)core::allocator_tag::memory_system
#endif

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
	this->active_list = core::memory::registry(max_allowed_allocations);
	this->free_list   = core::memory::registry(max_allowed_allocations);

	this->lock = false;

	CORE_DEBUG(0, "new memory_block is created for {} , with size {}byte", this->tag , this->block_size);
}

/*
	destructor
*/

memory_block::~memory_block() NOEXP {
	
	if (this->start) {
		u32 active_memory = this->active_list.get_allocations_size();
		u32 active_allocations = this->active_list.get_allocations_count();

	#ifdef DEBUG // detected memory leaks
		if (active_memory || active_allocations) {
			CORE_WARN(CORE_LOG_CONFIG_ALL, "memory leak detected , ({}leak/{}byte) !");
		}
	#endif

		// free memory block
		core::memory::deallocate(this->start);
	}

}

/*
	memory_block public functions
*/


} // namespace core end

#endif