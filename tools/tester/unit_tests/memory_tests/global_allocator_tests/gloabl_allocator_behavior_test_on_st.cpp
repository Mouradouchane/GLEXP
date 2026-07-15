#pragma once

#ifndef GLOBAL_ALLOCATOR_BEHAVIOR_TEST_ON_ST_CPP
#define GLOBAL_ALLOCATOR_BEHAVIOR_TEST_ON_ST_CPP

#include "tools/tester/macros.hpp"

#include "core/memory/memory.hpp"

#include "gloabl_allocator_behavior_test_on_st.hpp"


bool global_allocator_simple_behavior_test_on_st_1() NOEXP {

	g_memory_handle h1 = core::memory::allocate(
		g_memory_request{ 
			.size = 32 MB , 
			.tag = allocator_tag::debug_system
		}
	);

	if (h1.get_pointer() == nullptr) return TEST_FAIL;
	if (h1.response() != allocator_response::success) return TEST_FAIL;

	u64 size = core::memory::current_memory_usage(allocator_tag::debug_system);
	if (size != (32 MB)) return TEST_FAIL;

	g_memory_handle h2 = core::memory::allocate(
		g_memory_request{
			.size = 32 MB ,
			.tag = allocator_tag::debug_system
		}
	);

	if (h2.get_pointer() == nullptr) return TEST_FAIL;
	if (h2.response() != allocator_response::success) return TEST_FAIL;

	size = core::memory::current_memory_usage(allocator_tag::debug_system);
	if (size != (64 MB)) return TEST_FAIL;

	size = core::memory::current_memory_usage(allocator_tag::graphics_system);
	if (size != 0) return TEST_FAIL;
	
	core::memory::deallocate(h1);

	size = core::memory::current_memory_usage(allocator_tag::debug_system);
	if (size != (32 MB)) return TEST_FAIL;

	core::memory::deallocate(h2);

	size = core::memory::current_memory_usage(allocator_tag::debug_system);
	if (size != 0) return TEST_FAIL;

	return TEST_PASS;
}


bool global_allocator_simple_behavior_test_on_st_2() NOEXP {

	g_memory_handle handles[12]  = { g_memory_handle{} };

	// allocate memory
	for (u8 i = 0; i < 12; i++) {
		handles[i] = core::memory::allocate(
			g_memory_request{
				.size = 8 MB,
				.tag = (allocator_tag)i
			}
		);
	}

	u64 size = core::memory::total_memory_usage();
	if(size != (12 * 8 MB)) return TEST_FAIL;
	
	u64 psize = core::memory::peak_memory_usage();
	if(size != psize) return TEST_FAIL;

	core::memory::deallocate(handles[0]);
	if (core::memory::current_memory_usage(allocator_tag::unkown) != 0) return TEST_FAIL;

	// deallocate memory
	for (u8 i = 1; i < 12; i++) {
		core::memory::deallocate(handles[i]);
	}

	size = core::memory::total_memory_usage();
	if (size != 0) return TEST_FAIL;

	psize = core::memory::peak_memory_usage();
	if (psize != (12 * 8 MB)) return TEST_FAIL;

	return TEST_PASS;
}

bool global_allocator_allocate2_simple_behavior_test_on_st() NOEXP {

	g_memory_handle_2 h1 = core::memory::allocate_tow(
		g_memory_request{ .size = 32 MB , .tag = allocator_tag::debug_system },
		g_memory_request{ .size = 32 MB , .tag = allocator_tag::debug_system }
	);

	if(h1.handle_1.response() != h1.handle_2.response()) return TEST_FAIL;
	u64 t = (2 * 32 MB);
	u64 size = core::memory::total_memory_usage();
	u64 peak = core::memory::peak_memory_usage();

	if(size != t) return TEST_FAIL;
	if(peak < t)  return TEST_FAIL;

	u64 current = core::memory::current_memory_usage(allocator_tag::debug_system);
	if(current != t) return TEST_FAIL;

	core::memory::deallocate(h1.handle_1);
	current = core::memory::current_memory_usage(allocator_tag::debug_system);

	if(current != (32 MB)) return TEST_FAIL;

	core::memory::deallocate(h1.handle_2);
	current = core::memory::current_memory_usage(allocator_tag::debug_system);

	if(current != 0) return TEST_FAIL;
	
	return TEST_PASS;
}

#endif