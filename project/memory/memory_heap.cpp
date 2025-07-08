/*
	memory heap implementation
*/
#pragma once

#ifndef MEMORY_HEAP_CPP
#define MEMORY_HEAP_CPP

#include "assert.hpp"
#include "memory_heap.hpp"

heap::heap(
	u32 size, u32 max_allocation, allocation_section heap_usage
) {
	DEBUG_BREAK;
	
	// few checks first
	CRASH_AT_FALSE(size, "memory/heap: zero size_ allocation not allowed !");
	CRASH_AT_FALSE(size < heap::minimum_heap_size_allowed , "memory/heap: less than 1KB heap is not allowed");
	CRASH_AT_FALSE(size > heap::maximum_heap_size_allowed , "memory/heap: bigger than 1GB heap is not allowed");
	CRASH_AT_FALSE(max_allocation, "memory/heap: zero size allocation list in not allowed")
	
	// init heap memory 
	this->size  = size;
	this->start = (byte*)memory::alloc(size, heap_usage);
	this->end   = (byte*)(this->start) + size;
	this->seek  = this->start;
	this->section = heap_usage;

	// init heap registers
	alloc_list.list = (register_info*)memory::alloc(sizeof(register_info)*max_allocation , heap_usage);
	free_list.list  = (register_info*)memory::alloc(sizeof(register_info)*max_allocation , heap_usage);
	alloc_list.size = max_allocation;
	free_list.size  = max_allocation;

}

// todo: implement destruction
heap::~heap() {

}

/*
	heap public function's
*/

void* heap::allocate(u32 size_) {
	DEBUG_BREAK;
	this->lock();
	
	void* pointer = nullptr;
	u32 free = (this->end - this->seek);

	CRASH_AT_FALSE(size_ , "heap.allocate: 0 size_ allocation not allowed !");
	CRASH_AT_TRUE(size_ > free, "heap.allocate: no memory left for allocation !");
	
	// try to do "first-fit" based allocation if seek not lapped
	if (!this->is_lapped && free >= size_) {
		pointer = this->seek;
		// update heap variables
		this->seek  += size_;
		this->alloc += size_;
		
		if (this->seek >= this->end) {
			this->is_lapped = true;
		}

		// update alloc_list
		if (alloc_list.range < alloc_list.size) {

		}
		else CRASH_AT_FALSE(0, "heap.allocate: ");
		this->unlock();
		return pointer;
	}

	// else try to do look-up based allocation
	if ( 
		(free_list.size  - free_list.range)  > 1 && 
		(alloc_list.size - alloc_list.range) > 0 
	) {

		// o(n) ! linear look_up for free spot 
		for (u32 i = 0; i < free_list.range; i++) {

			if (free_list.list[i].size >= size_) {
				register_info chunk = free_list.list[i];

				// both registers need update
				if (chunk.size > size_) {
					this->alloc -= (chunk.size - size_);

					// update free_list
					free_list.list[i].size -= size_;

					// update alloc_list
					free_list.range += 1;
					free_list.list[free_list.range] = ;
				}

				return chunk.pointer;
			}
		}
	}


	// no memory left for allocation
	CRASH_AT_TRUE(0, "heap.allocate: no memory left for allocation !");
}

// todo: implement deallocation
void heap::deallocate(void* pointer) {
	ASSERT_EXP(0);

}


u32 heap::heap_size(memory_unit return_value_unit) noexcept {
	
	switch (return_value_unit) {
	case memory_unit::KB:{
		return BYTE_TO_KB(this->size);
	}	
		default: return this->size;
	}
}

u32 heap::allocated(memory_unit return_value_unit) noexcept {
	return this->alloc;
}

u32 heap::available(memory_unit return_value_unit) noexcept {
	return this->size - this->alloc;
}

/*
	heap private function's
*/ 

// todo : find better solution if possible -> "thread-id lock/unlock maybe"
// note : this is might be so bad !
void heap::lock() {
	while (this->is_locked);
	this->is_locked = true;
}

void heap::unlock() {
	this->is_locked = false;
}

// this function merge deallocated areas who are next to each others ,
// it basically merge them into one area 
void heap::merge_free_areas() {

	// todo : implement
	ASSERT_EXP(0);
}


#endif