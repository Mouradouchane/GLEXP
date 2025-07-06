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
	CRASH_AT_FALSE(size, "memory/heap: zero size allocation not allowed !");
	CRASH_AT_FALSE(size < heap::minimum_heap_size_allowed , "memory/heap: less than 1KB heap is not allowed");
	CRASH_AT_FALSE(size > heap::maximum_heap_size_allowed , "memory/heap: bigger than 1GB heap is not allowed");
	CRASH_AT_FALSE(max_allocation, "memory/heap: zero size allocation list in not allowed")
	
	// init heap memory 
	this->size  = size;
	this->free  = size;
	this->start = (byte*)memory::alloc(size, heap_usage);
	this->end   = (byte*)(this->start) + size;
	this->seek  = this->start;
	this->section    = heap_usage;

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

void* heap::allocate(u32 size) {
	DEBUG_BREAK;
	
	CRASH_AT_FALSE(size , "heap.alloc: 0 size allocation not allowed !");
	CRASH_AT_TRUE(size > this->free, "heap.alloc: no memory left for allocation !");
	
	this->lock();
	void* pointer = nullptr;

	if (this->lapped) {
		// do lookup based allocation
	}

	if ((this->end - this->seek) >= size) {
		// else do linear based allocation
		pointer = this->seek;
		this->seek += size;
	}
	else {
		
	}

	this->unlock();

}

// todo: implement deallocation
void heap::deallocate(void* pointer) {
	ASSERT_EXP(0);

}


u32 heap::heap_size(memory_unit return_value_unit) noexcept {
	return this->size;
}

u32 heap::allocated(memory_unit return_value_unit) noexcept {
	return this->alloc;
}

u32 heap::available(memory_unit return_value_unit) noexcept {
	return this->free;
}

/*
	heap private function's
*/ 

bool heap::lock() {
	if (this->locked) return false;

	this->locked = true;
	return true;
}

bool heap::unlock() {
	if (!this->locked) return false;

	this->locked = false;
	return true;
}

// this function merge deallocated areas who are next to each others ,
// it basically merge them into one area 
void heap::merge_free_areas() {

	// todo : implement
	ASSERT_EXP(0);
}


#endif