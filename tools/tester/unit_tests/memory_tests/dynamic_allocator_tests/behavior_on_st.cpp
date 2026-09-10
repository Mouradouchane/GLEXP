#if 1
#pragma once 

#ifndef UNIT_TEST_BEHAVIOR_ON_ST_DYNAMIC_ALLOCATOR_CPP
#define UNIT_TEST_BEHAVIOR_ON_ST_DYNAMIC_ALLOCATOR_CPP

#include "tools/tester/macros.hpp"

#include "core/memory/memory.hpp"
#include "core/memory/dynamic/dynamic_allocator.hpp"

#include "behavior_on_st.hpp"

bool dynamic_allocator_simple_usage_on_st_test_1() {
	
	dynamic_allocator_parameters parameters;
	parameters.tag = subsystem_memory_tag::debug_system;
	parameters.max_allocations_per_block = 512;
	parameters.memory_budget = 32 MB;
	parameters.allocate_all_at_once = false;
	parameters.name = "tester allocator 1";
	parameters.is_multi_thread = false;
	
	core::dynamic_allocator dc(parameters);

	if (dc.name() != "tester allocator 1") { DEBUG_FAILUER(); }
	if(dc.tag() != subsystem_memory_tag::debug_system) { DEBUG_FAILUER(); }

	if (dc.size() != 64 KB) { DEBUG_FAILUER(); }

	u32 size = parameters.memory_budget / 4;

	memory_handle h1 = dc.allocate(size, memory_tag::dev);
	memory_handle h2 = dc.allocate(size, memory_tag::ai);
	memory_handle h3 = dc.allocate(size, memory_tag::dev);
	memory_handle h4 = dc.allocate(size, memory_tag::ai);

	if (dc.current_memory_usage(memory_tag::dev) != (size * 2)) { DEBUG_FAILUER(); }
	if (dc.current_memory_usage(memory_tag::ai)  != (size * 2)) { DEBUG_FAILUER(); }

	return TEST_PASS;
}

bool dynamic_allocator_simple_usage_on_st_test_2() {
	return false;
}


bool dynamic_allocate_memory_1() {
	return false;
}

bool dynamic_allocate_memory_2() {
	return false;
}

#endif
#endif