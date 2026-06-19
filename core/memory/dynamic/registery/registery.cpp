#pragma once 

#ifndef CORE_MEMORY_REGISTRE_CPP
#define CORE_MEMORY_REGISTRE_CPP

#include <vector>

#include "core/logger/logger.hpp"
#include "registery.hpp"

#ifdef DEBUG
	static auto _core_registry_alloc_logger_ = CORE_GET_LOGGER(MEMORY_ALLOCATOR_LOGGER);
#else 
	static auto _core_registry_alloc_logger_ = nullptr;
#endif

#define _LOGGER_  _core_registry_alloc_logger_

namespace core {

/*
		constructor
*/ 

memory_registry::memory_registry() NOEXP {

	this->capacity = memory_registry::default_register_capacity;
	this->size = sizeof(core::memory_allocation) * this->capacity;

	this->list = (core::memory_allocation*) core::memory::allocate(
		core::g_memory_request {
			this->size, 
			(u8)core::memory_tag::registry
		}
	);

	this->allocations_count = 0;
	this->allocations_size  = 0;

	if (this->list) {
		CORE_DEBUG(0,"new registry created with capacity={} .", this->capacity);
	}

}

memory_registry::memory_registry(u32 register_capacity) NOEXP {

	this->capacity = (register_capacity) ? register_capacity : memory_registry::default_register_capacity;
	this->size = sizeof(core::memory_allocation) * this->capacity;

	this->list = (core::memory_allocation*)core::memory::allocate(
		core::g_memory_request {
			this->size,
			(u8)core::memory_tag::registry
		}
	);

	this->allocations_count = 0;
	this->allocations_size  = 0;
			
	if (this->list) {
		CORE_DEBUG(0, "new registry created with capacity={} .", this->capacity);
	}
}

/*
	destructor
*/

memory_registry::~memory_registry() NOEXP {

	// [WARNNING] : any allocations left registred in register will be lost !
	core::memory::deallocate(this->list);

	this->size = 0;

	CORE_DEBUG(0,"registry with capacity={} desturcted !", this->capacity);
	this->capacity = 0;

}


/*
	registry public functions
*/

u32 memory_registry::insert(void* ptr, u32 size, u8 tag) NOEXP {

	if (this->allocations_count < this->capacity) {

		// hash then search for empty place to insert new allocation
		u32 index = this->hash_pointer(ptr);
		u32 i = this->search(index, nullptr);

		if (i < this->capacity) {
		#ifdef DEBUG
			this->list[i] = core::memory_allocation{ ptr , size , tag };
		#else 
			this->list[i] = memory_allocation{ ptr , size };
		#endif

			this->allocations_count += 1;
			this->allocations_size  += size;

			return i;
		}
		else { // failed to find empty place in registry 
			CORE_FATAL(CORE_LOG_CONFIG_ALL, REGISTRY_FAILED_TO_INSERT, core::pointer_to_hex_string(ptr), size);
			return this->capacity;
		}

	}
	else {
		CORE_WARN(CORE_LOG_CONFIG_ALL, REGISTRY_IS_FULL);
		return this->capacity;
	}

}


bool memory_registry::remove(void* ptr) NOEXP {

	// hash then search for ptr
	u32 index = this->hash_pointer(ptr);
	u32 i = this->search(index, ptr);

	if (i < this->capacity) {
		this->allocations_count -= 1;
		this->allocations_size  -= this->list[i].size;

		this->list[i].clear();

		return true;
	}
	else { // ptr not found
		CORE_WARN(CORE_LOG_CONFIG_ALL, REGISTRY_PTR_NOT_FOUND, core::pointer_to_hex_string(ptr));
		return false;
	}

}


core::memory_allocation memory_registry::cut(void* ptr) NOEXP {

	// hash then search for ptr
	u32 index = this->hash_pointer(ptr);
	u32 i = this->search(index, ptr);

	if (i < this->capacity) {
		core::memory_allocation alloc = this->list[i];

		this->allocations_count -= 1;
		this->allocations_size  -= this->list[i].size;

		this->list[i].clear();

		return alloc;
	}
	else { // ptr not found
		CORE_WARN(CORE_LOG_CONFIG_ALL, REGISTRY_PTR_NOT_FOUND, core::pointer_to_hex_string(ptr));
		return core::memory_allocation{ 0 };
	}

}

u32 memory_registry::exist(void* ptr) NOEXP {
	return this->search(this->hash_pointer(ptr), ptr);
}

INLINE u32 memory_registry::get_size() NOEXP {
	return this->size;
}

INLINE u32 memory_registry::get_capacity() NOEXP {
	return this->capacity;
}

INLINE u32 memory_registry::get_allocations_count() NOEXP {
	return this->allocations_count;
}

INLINE u32 memory_registry::get_allocations_size()  NOEXP {
	return this->allocations_size;
}

// note: this could be a disaster on preformance level because it O(N)
core::i_memory_allocation memory_registry::get_allocation(u32 target_size) NOEXP {

	for (u32 i = 0; i < this->capacity; i++) {

		if (this->list[i].size >= target_size) {
			return core::i_memory_allocation { 
				this->list[i].ptr,
				this->list[i].size,
				i
			};
		}

	}

	return core::i_memory_allocation{ 0 };
}

core::i_memory_allocation memory_registry::get_biggest_allocation(u32 target_size) NOEXP {

	core::memory_allocation allocation = this->list[this->biggest_allocation];

	if (allocation.size >= target_size) {
		return core::i_memory_allocation{ allocation.ptr , allocation.size , this->biggest_allocation };
	}
	
	return core::i_memory_allocation{ 0 };
}


void memory_registry::merge_free_areas() NOEXP {

	// copy list
	std::vector<core::memory_allocation> c_list(this->list , this->list + this->capacity);

	// sort allocations by address
	std::sort(
		c_list.begin(), c_list.end(), 
		[&](core::memory_allocation const& A , core::memory_allocation const& B) -> bool {
			return (A.ptr < B.ptr);
		}
	);

	// start the merge process
	for (u32 i = 0; i < (c_list.size() - 1); i++) {
		
		// if tow allocations contigues in list
		if (c_list[i + 1].ptr == ((byte*)c_list[i].ptr + c_list[i].size)) {
			// merge both of them
			c_list[i + 1].ptr   = c_list[i].ptr;
			c_list[i + 1].size += c_list[i].size;

			// empty current allocation
			c_list[i] = core::memory_allocation{ 0 };
		}

	}

	// empty registry list
	std::memset(this->list, 0, this->size);

	core::i_memory_allocation new_biggest = { 0 };

	// insert back the new once
	for (u32 i = 0; i < c_list.size(); i++) {

		if (c_list[i].ptr) {
			u32 index = this->search( this->hash_pointer(c_list[i].ptr) , nullptr);

			if (index < this->capacity) {
				// direct insert
				this->list[i] = c_list[i];

				// keep track of the biggest allocation
				if (this->list[i].size > new_biggest.size) {
					new_biggest = core::i_memory_allocation {
						this->list[i].ptr,
						this->list[i].size,
						i
					};
				}
			}

		}

	}

}


/*
	registry helper functions
*/

// todo: get some good hashing function later -> https://github.com/cyan4973/xxhash
u32 memory_registry::hash_pointer(void* ptr) NOEXP {

	return u32((u64)ptr % this->capacity);
}

u32 memory_registry::search(u32 start_index, void* ptr) NOEXP {

	u32 i = start_index;

	// search from start_index to end
	for (; i < this->capacity; i++) {
		if (this->list[i].ptr == ptr) return i;
	}

	// search from 0 to start_index
	i = 0;
	for (; i < start_index; i++) {
		if (this->list[i].ptr == ptr) return i;
	}

	// not found
	return this->capacity;
}



} // namesapce core end

#endif