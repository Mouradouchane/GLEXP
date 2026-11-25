#if 1
#pragma once 

#ifndef UNIT_TEST_BEHAVIOR_ON_ST_DYNAMIC_ALLOCATOR_CPP
#define UNIT_TEST_BEHAVIOR_ON_ST_DYNAMIC_ALLOCATOR_CPP

#include "tools/tester/macros.hpp"

#include "core/memory/dynamic_allocator/dynamic_allocator.hpp"

#include "behavior_on_st.hpp"

bool dynamic_allocator_simple_behavior_test_1() {
	
	core::dynamic_allocator dc("tester allocator 1",4 KB,2,16,subsystem_memory_tag::debug_system);

	if (dc.name() != "tester allocator 1") { DEBUG_FAILUER(); }
	if (dc.tag() != subsystem_memory_tag::debug_system) { DEBUG_FAILUER(); }

	block_status bs0 = dc.memory_block_status(0);
	block_status bs1 = dc.memory_block_status(1);

	if (bs0.allocated_memory != (4 KB)) { DEBUG_FAILUER(); }
	if (bs1.allocated_memory != (4 KB)) { DEBUG_FAILUER(); }
	if (dc.total_memory() != (8 KB)){ DEBUG_FAILUER(); }
	
	if (dc.free_memory() != (8 KB)) { DEBUG_FAILUER(); }
	
	u32 size = (4 KB) / 4;

	memory_handle h1 = dc.allocate(size, memory_tag::dev, 0, true);
	memory_handle h2 = dc.allocate(size, memory_tag::ai , 1, true);
	memory_handle h3 = dc.allocate(size, memory_tag::dev, 1, true);
	memory_handle h4 = dc.allocate(size, memory_tag::ai , 0, true);

	if (dc.allocated_memory(memory_tag::dev) != (size * 2)) { DEBUG_FAILUER(); }
	if (dc.allocated_memory(memory_tag::ai)  != (size * 2)) { DEBUG_FAILUER(); }
	if (dc.allocated_memory() != (4 KB)) { DEBUG_FAILUER(); }
	if (dc.free_memory() != (4 KB)) { DEBUG_FAILUER(); }

	dc.deallocate(h1);
	dc.deallocate(h2);
	dc.deallocate(h3);
	dc.deallocate(h4);

	if (dc.allocated_memory(memory_tag::dev) != 0) { DEBUG_FAILUER(); }
	if (dc.allocated_memory(memory_tag::ai)  != 0) { DEBUG_FAILUER(); }
	if (dc.allocated_memory() != 0) { DEBUG_FAILUER(); }
	if (dc.free_memory() != (8 KB)) { DEBUG_FAILUER(); }

	return TEST_PASS;
}

struct test_struct {
	u32  x , y , z;
};

bool dynamic_allocator_all_blocks_same_size() {

	core::dynamic_allocator dc("tester allocator:all block same size",4 KB,2,16,subsystem_memory_tag::debug_system);

	memory_handle handles[16] = { };

	// allocate memory
	for (u8 i = 0; i < 16; i += 1) {
		handles[i] = dc.allocate(sizeof(test_struct),(memory_tag)i,0,false,true);
		test_struct* ptr = (test_struct*)handles[i].pointer();
		*ptr = test_struct{1,2,3};
	}

	// check allocated size
	if (dc.allocated_memory() != (sizeof(test_struct) * 16)) {
		DEBUG_FAILUER();
	}

	// check elements in memory
	for (u8 i = 0; i < 16; i += 1) {
		test_struct* ptr = (test_struct*)handles[i].pointer();
		test_struct st = *ptr;

		if (st.x != 1 && st.y != 2 && st.z != 3) {
			DEBUG_FAILUER();
		}
	}

	return TEST_PASS;
}

bool dynamic_allocator_blocks_with_different_sizes() {
	return false;
}

#endif
#endif