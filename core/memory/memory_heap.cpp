/*
	global_memory memory_heap implementation
*/
#pragma once

#ifndef MEMORY_HEAP_CPP
#define MEMORY_HEAP_CPP

#include "core/assert.hpp"
#include "memory_heap.hpp"

#include <algorithm>

/*
	constructor
*/
core::memory_heap::memory_heap(
	u32 heap_size, 
	u32 max_allocation, 
	memory_usage heap_usage
){

	// TODO: change to logger
	// few checks first
	CRASH_IF(heap_size < 1, "memory_heap: zero size memory_heap not allowed !");
	CRASH_IF(
		(heap_size < core::memory_heap::minimum_heap_size_allowed) ||
		(heap_size > core::memory_heap::maximum_heap_size_allowed), 
		// todo: change it to better message later 
		// when you have logger
		"memory_heap: memory_heap size not allowed !"
	);
	CRASH_IF(max_allocation < 1, "memory_heap: max allowed allocations cannot be 0 !");
	// note: maybe this need to be a logger warning
	CRASH_IF(heap_size < max_allocation, "memory_heap: max allowed allocations larger than the actuall global_memory !");
	
	// init memory_heap variables
	this->max_allowed_allocations = max_allocation;
	this->section   = heap_usage;
	this->heap_size = heap_size;
	this->start     = (byte*)global_memory::allocate(heap_size, heap_usage);
	this->end       = (byte*)(this->start + this->heap_size);
	this->seek      = this->start;

	// init allocate/deallocate lists
	this->alloc_list_size = (this->max_allowed_allocations + u32(this->max_allowed_allocations / 2u));
	
	this->alloc_list = (registry_pair*)global_memory::allocate(sizeof(registry_pair) * this->alloc_list_size);
	this->free_list  = (registry_pair*)global_memory::allocate(sizeof(registry_pair) * this->max_allowed_allocations);
	
	init_registry_list(this->alloc_list, this->alloc_list_size);
	init_registry_list(this->free_list, this->max_allowed_allocations);
}

// TODO: call elements destructor's if possible
core::memory_heap::~memory_heap() {
	global_memory::deallocate( this->start );
	global_memory::deallocate( this->alloc_list );
	global_memory::deallocate( this->free_list );

	this->start = nullptr;
	this->end   = nullptr;
	this->seek  = nullptr;

	this->alloc_list = nullptr;
	this->free_list  = nullptr;
}

/*
	memory_heap public static function's
*/

f32 core::memory_heap::minimum_size_allowed(memory_unit return_value_unit) noexcept {
	switch (return_value_unit) {
		case memory_unit::kb : return BYTE_TO_KB(core::memory_heap::minimum_heap_size_allowed);
		case memory_unit::mb : return BYTE_TO_MB(core::memory_heap::minimum_heap_size_allowed);
		case memory_unit::gb : return BYTE_TO_GB(core::memory_heap::minimum_heap_size_allowed);
		default : return f32(core::memory_heap::minimum_heap_size_allowed);
	}
}

f32 core::memory_heap::maximum_size_allowed(memory_unit return_value_unit) noexcept {
	switch (return_value_unit) {
		case memory_unit::kb : return BYTE_TO_KB(core::memory_heap::maximum_heap_size_allowed);
		case memory_unit::mb : return BYTE_TO_MB(core::memory_heap::maximum_heap_size_allowed);
		case memory_unit::gb : return BYTE_TO_GB(core::memory_heap::maximum_heap_size_allowed);
		default: return f32(core::memory_heap::minimum_heap_size_allowed);
	}
}

/*
	memory_heap private function's
*/

inline void core::memory_heap::sort_list_by_address(registry_pair* list, u32 size) {

	std::sort(list, list + size, [&](const registry_pair& a, const registry_pair& b) -> bool {
		return a.pointer < b.pointer;
	});
}

inline void core::memory_heap::sort_list_by_size(registry_pair* list, u32 size) {

	std::sort(list, list + size, [&](const registry_pair& a, const registry_pair& b) -> bool {
		return a.size > b.size;
	});
}

inline void core::memory_heap::init_registry_list(registry_pair* list, u32 size) {

	for (u32 i = 0; i < size; i++) {
		*(list + i) = registry_pair{nullptr , NULL};
	}

}

inline void core::memory_heap::find_free_location(u32& index_output , u32 size__) {

	for (u32 i = 0; i < this->free_list_range; i++) {

		if (this->free_list[i].size >= size__) {
			index_output = i;
			return;
		}
	}

}

inline void core::memory_heap::allocate_from_free_list(void** pointer, u32 size__, u32 index) {

	registry_pair _allocation = this->free_list[index];

	*pointer = _allocation.pointer;

	if (_allocation.size > size__) {
		this->free_list[index].pointer = (byte*)this->free_list[index].pointer + size__;
		this->free_list[index].size -= size__;
	}
	else {
		this->free_list[index] = registry_pair{ nullptr, NULL};
	}

	register_allocation(*pointer , size__);
}

u32 core::memory_heap::hash_pointer(void* pointer) {
	return (u64)pointer % this->alloc_list_size;
}

inline void core::memory_heap::register_allocation(void* pointer, u32 size) {

	u32 index = hash_pointer(pointer);

	// look for empty spot to insert
	while ( index < this->alloc_list_size) {

		if (this->alloc_list[index].pointer == nullptr) {

			this->alloc_list[index] = registry_pair{ pointer , size };
			this->alloc_size += size;
			this->registered += 1;
			return;
		}
		index += 1;
	}

	u32 i = 0;
	while (i < index) {

		if (this->alloc_list[i].pointer == nullptr) {

			this->alloc_list[i] = registry_pair{ pointer , size };
			this->alloc_size += size;
			this->registered += 1;
			return;
		}
		i += 1;
	}

	// if no place found in alloc_list
	CRASH_IF(1, "core::memory_heap::register_allocation : allocation registry is full no place found for new insert !");
}

inline void core::memory_heap::unregister_allocation(u32 _index) {
	DEBUG_BREAK;

	this->free_list_range += 1;
	CRASH_IF(
		this->free_list_range > this->max_allowed_allocations , 
		"memory_heap.unregister_allocation : free_list if full , this is look like a bug !"
	);

	u32 i = 0;
	u32 size__ = this->alloc_list[_index].size;

	// look for right place to insert
	for (	; i < this->free_list_range; i++) {
		if (this->free_list[i].size < size__) break;
	}

	// make a room for new registry by shift few elements
	for (u32 r = this->free_list_range; r > i ; r--) {
		this->free_list[r-1] = this->free_list[r];
	}

	// insert
	this->free_list[i] = this->alloc_list[_index];
	
	// remove/unregister allocation
	this->alloc_size -= this->alloc_list[_index].size;
	this->alloc_list[_index] = registry_pair{ nullptr , NULL };
	this->registered -= 1;
}

// this function merge deallocated areas who are next to each others ,
// it basically merge them into one area 
void core::memory_heap::merge_free_areas() {

	this->sort_list_by_address(this->free_list, this->free_list_range);

	// merge process
	registry_pair* s_ptr = this->free_list + 1;
	registry_pair* e_ptr = this->free_list + this->free_list_range;
	registry_pair* marker_ptr = this->free_list;

	for (; s_ptr <= e_ptr; s_ptr++) {

		if (((byte*)marker_ptr->pointer + marker_ptr->size) == s_ptr->pointer) {

			marker_ptr->size += s_ptr->size;
			*s_ptr = registry_pair{ nullptr , NULL };

			continue;
		}
		else {
			marker_ptr = s_ptr;
		}

	}

	this->sort_list_by_size(this->free_list, this->free_list_range);
}


/*
	memory_heap public function's
*/

void* core::memory_heap::allocate(u32 size__) {
	CRASH_IF(size__ < 1, "memory_heap.allocate: 0 size allocation not allowed !");
	CRASH_IF(size__ > this->heap_size, "memory_heap.allocate: size of allocation asked for bigger than the memory_heap !");
	CRASH_IF(this->registered >= this->max_allowed_allocations, "memory_heap.allocate: max allowed allocations is reached !");
	
	void* pointer   = nullptr;
	u32  _available = (this->end >= this->seek) ? u32(this->end - this->seek) : 0u;

	if ( (this->seek < this->end) && (size__ <= _available) ) {
		// allocate from seek
		pointer = this->seek;
			
		// update variables
		this->seek += size__;

		// register the allocation
		this->register_allocation(pointer, size__);
			
		return pointer;
	}
	else {
		DEBUG_BREAK;
		u32 index = this->max_allowed_allocations;

		// search for empty spot
		this->find_free_location(index, size__);

		if (index < this->max_allowed_allocations) {
			allocate_from_free_list(&pointer, size__ ,index);
			return pointer;
		}
		/*
			else : try to merge empty spots is possible
			       and see if there's any spot for allocation
		*/ 
		this->merge_free_areas();

		// search again after the merge process
		this->find_free_location(index , size__);
		
		// if no place found : crash -> "no global_memory left"
		CRASH_IF(
			index >= this->max_allowed_allocations,
			"memory_heap.allocate: no memory left or found for allocation !"
		);

		allocate_from_free_list(&pointer, size__, index);
		return pointer;
	}

}

void core::memory_heap::deallocate(void* pointer){
	
	CRASH_IF(
		(pointer < this->start) || (pointer > this->end) ,
		"memory_heap.deallocate: invalid pointer " + pointer_to_hex_string((ptr64)pointer) + " , out of range !"
	);

	u32 index = this->hash_pointer(pointer);

	for (	; index < this->alloc_list_size; index++ ) {
		if (this->alloc_list[index].pointer == pointer) {
			unregister_allocation(index);
			return;
		}
	}

	for (u32 i = 0; i < index; i++) {
		if (this->alloc_list[index].pointer == pointer) {
			unregister_allocation(index);
			return;
		}
	}

	CRASH_IF(1, "memory_heap.deallocate: pointer " + pointer_to_hex_string((ptr64)pointer) + " not found allocated !");
}

u32 core::memory_heap::size() noexcept {
	return this->heap_size;
}

f32 core::memory_heap::size_f(memory_unit return_value_unit) noexcept {
	
	switch (return_value_unit) {
		case memory_unit::kb : return BYTE_TO_KB(this->heap_size);
		case memory_unit::mb : return BYTE_TO_MB(this->heap_size);
		case memory_unit::gb : return BYTE_TO_GB(this->heap_size);
	
		default: return f32(this->heap_size);
	}
}

u32 core::memory_heap::allocated(memory_unit return_value_unit) noexcept {
	return this->alloc_size;
}

u32 core::memory_heap::available(memory_unit return_value_unit) noexcept {
	return this->heap_size - this->alloc_size;
}


#endif