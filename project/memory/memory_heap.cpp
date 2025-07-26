/*
	memory heap implementation
*/
#pragma once

#ifndef MEMORY_HEAP_CPP
#define MEMORY_HEAP_CPP

#include "assert.hpp"
#include "memory_heap.hpp"

/*
	
*/

heap::heap(
	u32 heap_size, 
	u32 max_allocation, 
	ALLOCATION_SECTION heap_usage
){
	DEBUG_BREAK;
	
	// few checks first
	// TODO: change to logger
	CRASH_AT_TRUE(heap_size < 1, "heap: zero size heap not allowed !");
	CRASH_AT_TRUE(
		(heap_size < heap::minimum_heap_size_allowed) ||
		(heap_size > heap::maximum_heap_size_allowed), 
		// todo : change it to better message later 
		// when you have logger
		"heap: heap size not allowed !"
	);
	CRASH_AT_TRUE(max_allocation < 1, "heap: max allowed allocations cannot be 0 !");
	// note: maybe this need to be a logger warning
	CRASH_AT_TRUE(heap_size < max_allocation, "heap: max allowed allocations larger than the actuall memory !");
	
	// init heap variables
	this->max_allowed_allocations = max_allocation;
	this->section   = heap_usage;
	this->heap_size = heap_size;
	this->start     = (byte*)memory::alloc(heap_size, heap_usage);
	this->end       = (byte*)(this->start + this->heap_size);
	this->seek      = this->start;

	// init alloc/free lists
	this->alloc_list = (registry_pair*)memory::alloc(sizeof(registry_pair) * this->max_allowed_allocations);
	this->free_list  = (registry_pair*)memory::alloc(sizeof(registry_pair) * this->max_allowed_allocations);
	
}

// TODO: implement destruction , but how
heap::~heap() {
	memory::free( this->start );
	memory::free( this->alloc_list );
	memory::free( this->free_list );

	this->start = nullptr;
	this->end   = nullptr;
	this->seek  = nullptr;

	this->alloc_list = nullptr;
	this->free_list  = nullptr;
}

/*
	heap public function's
*/

inline void heap::register_allocation(void* pointer, u32 size) {

	u32 index = hash_pointer(pointer);

	// look for empty spot to inser
	while ( index < this->maximum_heap_size_allowed) {

		if (this->alloc_list[index].pointer == nullptr) {

			this->alloc_list[index] = registry_pair{ pointer , size };
			this->registered += 1;
			return;
		}
		index += 1;
	}

	u32 i = 0;
	while (i < index) {

		if (this->alloc_list[i].pointer == nullptr) {

			this->alloc_list[i] = registry_pair{ pointer , size };
			this->registered += 1;
			return;
		}
		i += 1;
	}

	// if no place found in alloc_list
	CRASH_AT_TRUE(1, "heap::register_allocation : allocation registry is full no place found for new insert !");
}

void* heap::allocate(u32 _size) {
	CRASH_AT_TRUE(_size < 1, "heap.allocate: 0 size allocation not allowed !");
	CRASH_AT_TRUE(_size > this->heap_size, "heap.allocate: size of allocation asked for bigger than the heap !");
	CRASH_AT_TRUE(this->registered >= this->max_allowed_allocations, "heap.allocate: max allowed allocations is reached !");
	
	DEBUG_BREAK;
	void* pointer   = nullptr;
	u32  _available = (this->end >= this->seek) ? (this->end - this->seek) : 0;

	if ( (this->seek < this->end) && (_size <= _available) ) {
		// allocate from seek
		pointer = this->seek;
			
		// update variables
		this->seek       += _size;
		this->alloc_size += _size;

		// register the allocation
		this->register_allocation(pointer, _size);
			
		return pointer;
	}
	else {
		// do look-up for a free place
		// linear search for free place
		u32 index = 0;
		this->linear_lookup_for_allocation(index , _size);

		// if no place is found run "merge_free_areas"
		this->merge_free_areas();

		// try again 
		this->linear_lookup_for_allocation(index , _size);
		
		//if no place found -> crash "no memory left"
		CRASH_AT_TRUE(0 , "heap.allocate: no memory left for allocation !")
		
		// allocate it or allocate portion of it
		// register the allocation
		this->register_allocation(pointer, _size);
		
		// update variables

		return pointer;
	}

}

// todo: implement deallocation
void heap::deallocate(void* pointer){
	ASSERT_EXP(0);

}

u32 heap::size() noexcept {
	return this->heap_size;
}

f32 heap::size(MEMORY_UNIT return_value_unit) noexcept {
	
	switch (return_value_unit) {
		case MEMORY_UNIT::kb : return BYTE_TO_KB(this->heap_size);
		case MEMORY_UNIT::mb : return BYTE_TO_MB(this->heap_size);
		case MEMORY_UNIT::gb : return BYTE_TO_GB(this->heap_size);
	
		default: return f32(this->heap_size);
	}
}

u32 heap::allocated(MEMORY_UNIT return_value_unit) noexcept {
	return this->alloc_size;
}

u32 heap::available(MEMORY_UNIT return_value_unit) noexcept {
	return this->heap_size - this->alloc_size;
}

/*
	heap private function's
*/ 

// this function merge deallocated areas who are next to each others ,
// it basically merge them into one area 
void heap::merge_free_areas() {
	// todo : implement
	ASSERT_EXP(0);

}


#endif