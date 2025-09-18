#pragma once 

#ifndef UNIT_TEST_HEAP_CPP
#define UNIT_TEST_HEAP_CPP

#define UNIT_TEST

#include "heap_tests.hpp"
#include "core/memory/memory_heap.hpp"

bool create_and_destroy_heap_test_1() {

	heap test_heap(heap::minimum_size_allowed(MEMORY_UNIT::byte) , 512);

	if(test_heap.size() != heap::minimum_size_allowed(MEMORY_UNIT::byte)) return false;
	if(test_heap.allocated(MEMORY_UNIT::byte) != 0) return false;
	if(test_heap.available(MEMORY_UNIT::byte) != test_heap.size()) return false;

	return true;
}

bool create_and_destroy_heap_test_2() {
	return false;
}

#endif