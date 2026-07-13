#pragma once

#ifndef GLOBAL_ALLOCATOR_BEHAVIOR_TEST_ON_ST_CPP
#define GLOBAL_ALLOCATOR_BEHAVIOR_TEST_ON_ST_CPP

#include "core/memory/memory.hpp"
#include "tools/tester/macros.hpp"
#include "gloabl_allocator_behavior_test_on_st.hpp"


bool global_allocator_simple_behavior_test_on_st_1() NOEXP {

	void* h1 = core::memory::allocate(core::g_memory_request{ .size = 32 MB , .tag = (u8)core::memory_tag::dev });
	return false;
}


bool global_allocator_simple_behavior_test_on_st_2() NOEXP {
	return false;
}

#endif