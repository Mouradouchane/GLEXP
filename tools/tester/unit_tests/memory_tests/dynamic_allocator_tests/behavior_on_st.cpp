#if 1
#pragma once 

#ifndef UNIT_TEST_BEHAVIOR_ON_ST_DYNAMIC_ALLOCATOR_CPP
#define UNIT_TEST_BEHAVIOR_ON_ST_DYNAMIC_ALLOCATOR_CPP

#include "tools/tester/macros.hpp"

#include "core/memory/memory.hpp"
#include "core/memory/dynamic/dynamic_allocator.hpp"

#include "behavior_on_st.hpp"

bool dynamic_allocator_simple_behavior_test_1() {
	
	core::dynamic_allocator dc("tester allocator 1",4 KB,2,16,subsystem_memory_tag::debug_system);

	if (dc.name() != "tester allocator 1") { DEBUG_FAILUER(); }
	if (dc.tag() != subsystem_memory_tag::debug_system) { DEBUG_FAILUER(); }

	if (dc.block_size(0) != (4 KB)) { DEBUG_FAILUER(); }
	if (dc.block_size(1) != (4 KB)) { DEBUG_FAILUER(); }
	if (dc.memory_budget() != (8 KB)){ DEBUG_FAILUER(); }
	
	if (dc.free_memory() != (8 KB)) { DEBUG_FAILUER(); }
	
	u32 size = (4 KB) / 4;

	memory_handle h1 = dc.allocate(size, memory_tag::dev, 0, true);
	memory_handle h2 = dc.allocate(size, memory_tag::ai , 1, true);
	memory_handle h3 = dc.allocate(size, memory_tag::dev, 1, true);
	memory_handle h4 = dc.allocate(size, memory_tag::ai , 0, true);

	if (dc.current_memory_usage(memory_tag::dev) != (size * 2)) { DEBUG_FAILUER(); }
	if (dc.current_memory_usage(memory_tag::ai)  != (size * 2)) { DEBUG_FAILUER(); }
	if (dc.current_memory_usage() != (4 KB)) { DEBUG_FAILUER(); }
	if (dc.free_memory() != (4 KB)) { DEBUG_FAILUER(); }

	dc.deallocate(h1);
	dc.deallocate(h2);
	dc.deallocate(h3);
	dc.deallocate(h4);

	if (dc.current_memory_usage(memory_tag::dev) != 0) { DEBUG_FAILUER(); }
	if (dc.current_memory_usage(memory_tag::ai)  != 0) { DEBUG_FAILUER(); }
	if (dc.current_memory_usage() != 0) { DEBUG_FAILUER(); }
	if (dc.free_memory() != (8 KB)) { DEBUG_FAILUER(); }

	return TEST_PASS;
}


bool dynamic_allocator_all_blocks_same_size() {
	return false;
}

bool dynamic_allocator_blocks_with_different_sizes() {
	return false;
}

#endif
#endif