#pragma once 

#ifndef CORE_MEMORY_REGISTRE_CPP
#define CORE_MEMORY_REGISTRE_CPP

#include "registery.hpp"
#include "core/memory/global_allocator/global_allocator.hpp"
#include "core/logger/logger.hpp"

#ifdef DEBUG
	static auto _core_registry_alloc_logger_ = CORE_GET_LOGGER(MEMORY_ALLOCATOR_LOGGER);
#else 
	static auto _core_registry_alloc_logger_ = nullptr;
#endif

#define _LOGGER_  _core_registry_alloc_logger_

#define EMPTY_PTR (void*)-1

#define NAMESPACE core::memory

#define ZERO_SIZE_REGISTRY "zero size registry is not allowed , auto adjust to default size ."
#define REGISTRY_FAILED_TO_INSERT "registry failed to insert ({},{}) !"
#define REGISTRY_IS_FULL        "registery is full , failed to insert ."
#define REGISTRY_PTR_NOT_FOUND  "registry: pointer {} not found in registry ."


static INLINE u32 next_power_of_two(u32 value) NOEXP;


/*
		constructor
*/ 

NAMESPACE::registry::registry() NOEXP {

	this->capacity      = next_power_of_two(registry::default_register_size);
	this->capacity_bits = 63 - __builtin_clzll(this->capacity);

	this->size = sizeof(core::memory::allocation) * this->capacity;
	this->list = (core::memory::allocation*) core::memory::allocate(this->size , core::memory::tag::memory);

	CORE_INFO("registry with capacity={} constructed .", this->capacity);
}

NAMESPACE::registry::registry(u32 register_capacity) NOEXP {

	this->capacity      = next_power_of_two(register_capacity);
	this->capacity_bits = 63 - __builtin_clzll(this->capacity);

	this->size = sizeof(core::memory::allocation) * this->capacity;
	this->list = (core::memory::allocation*) core::memory::allocate( this->size, core::memory::tag::memory );

	CORE_INFO("registry with capacity={} constructed .", this->capacity);
}

/*
	destructor
*/

NAMESPACE::registry::~registry() NOEXP {

	core::memory::deallocate(this->list);

	this->size = 0;
	this->count = 0;
	this->capacity = 0;

	CORE_INFO("registry with capacity={} desturcted !", this->capacity);
}


/*
	registry public functions
*/

u32 NAMESPACE::registry::insert(void* ptr, u32 size, core::memory::tag tag_) NOEXP {

	if( this->count < (this->capacity - 1) ){

		// hash then search for empty place
		u32 index = this->hash_pointer(ptr);
		u32 i = this->search(index, nullptr);

		if (i < this->capacity) {
			#ifdef DEBUG
				this->list[i] = allocation{ ptr , size , tag_ };
			#else 
				this->list[i] = allocation{ ptr , size };
			#endif

			this->count += 1;
			this->allocations_size += size;

			return i;
		}
		else { // no place found
			CORE_ERROR(CORE_LOG_CONFIG_ALL, REGISTRY_FAILED_TO_INSERT , core::pointer_to_hex_string(ptr) , size);
			return this->count;
		}

	}
	else {
		CORE_WARN(CORE_LOG_CONFIG_ALL, REGISTRY_IS_FULL);
		return this->count;
	}

}


bool NAMESPACE::registry::remove(void* ptr) NOEXP {

	// hash then search for ptr
	u32 index = this->hash_pointer(ptr);
	u32 i = this->search(index , ptr);

	if (i < this->capacity) {
		this->count -= 1;
		this->allocations_size -= this->list[i].size;

		this->list[i].clear();

		return true;
	}
	else { // no place found
		CORE_WARN(CORE_LOG_CONFIG_ALL, REGISTRY_PTR_NOT_FOUND , core::pointer_to_hex_string(ptr));
		return false;
	}

}


core::memory::allocation NAMESPACE::registry::cut(void* ptr)NOEXP {

	// hash then search for ptr
	u32 index = this->hash_pointer(ptr);
	u32 i = this->search(index, ptr);

	if (i < this->capacity) {
		core::memory::allocation alloc = this->list[i];

		this->count -= 1;
		this->allocations_size -= this->list[i].size;

		this->list[i].clear();
	
		return alloc;
	}
	else {
		CORE_WARN(CORE_LOG_CONFIG_ALL, REGISTRY_PTR_NOT_FOUND, core::pointer_to_hex_string(ptr));
		return core::memory::allocation{ 0 };
	}

}

i64 NAMESPACE::registry::exist(void* ptr) NOEXP {

	return this->search( this->hash_pointer(ptr) , ptr);
}

INLINE u32 NAMESPACE::registry::get_size() NOEXP {
	return this->size;
}

INLINE u32 NAMESPACE::registry::get_capacity() NOEXP {
	return this->capacity;
}

INLINE u32 NAMESPACE::registry::get_allocations_count() NOEXP {
	return this->count;
}

INLINE u32 NAMESPACE::registry::get_allocations_size()  NOEXP {
	return this->allocations_size;
}


/*
	registry helper functions
*/

#define GOLDEN_RATIO 0x9e3779b97f4a7c15ULL

u32 NAMESPACE::registry::hash_pointer(void* ptr) NOEXP {

	u64 uptr = (u64)ptr;

	// todo[important] : check this "pointer-alignment" ?
	// strip the 3 low-order zero bits from "pointer alignment" !!!
	uptr = uptr >> 3;

	u64 hash = uptr * GOLDEN_RATIO;

	// shift the highest-entropy bits down to fit table size
	return (u32)(hash >> (64 - this->capacity_bits));
}

u32 NAMESPACE::registry::search(u32 start_index = 0 , void* ptr = nullptr) NOEXP {
	
	u32 i = start_index;

	// search from start_index to end
	for ( ; i < this->capacity; i++) {
		if (this->list[i].ptr == ptr) return i;	
	}

	// search from 0 to start_index
	i = 0;
	for ( ; i < start_index; i++) {
		if (this->list[i].ptr == ptr) return i;
	}

	// not found
	return this->capacity;
}


INLINE void NAMESPACE::allocation::clear() NOEXP {

#ifdef DEBUG
	this->ptr  = nullptr;
	this->size = 0;
	this->tag_ = tag::unkown;
#else 
	this->ptr = nullptr;
	this->size = 0;
#endif

}

static INLINE u32 next_power_of_two(u32 value) NOEXP {
	value--;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	value++;

	return value;
}

#undef NAMESPACE

#endif