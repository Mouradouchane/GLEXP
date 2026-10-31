#pragma once

#ifndef GLOBAL_ALLOCATOR_BEHAVIOR_TEST_ON_ST_CPP
#define GLOBAL_ALLOCATOR_BEHAVIOR_TEST_ON_ST_CPP

#include "tools/tester/macros.hpp"

#include "core/memory/memory.hpp"

#include "gloabl_allocator_behavior_test_on_st.hpp"

void write_to_memory(byte* s, byte* e) NOEXP;

bool global_allocator_simple_behavior_test_on_st_1() NOEXP {

	u64 total_size_snapshot = core::memory::total_memory_usage();

	memory_handle_g h1 = core::memory::allocate(
		memory_request_g{ 
			.size = 32 MB , 
			.tag = subsystem_memory_tag::debug_system
		}
	);

	if (h1.pointer() == nullptr) return TEST_FAIL;
	if (h1.response() != allocator_response::success) return TEST_FAIL;

	u64 size = core::memory::current_memory_usage(subsystem_memory_tag::debug_system);
	if (size < (32 MB)) return TEST_FAIL;

	memory_handle_g h2 = core::memory::allocate(
		memory_request_g{
			.size = 32 MB ,
			.tag = subsystem_memory_tag::debug_system
		}
	);

	if (h2.pointer()  == nullptr) return TEST_FAIL;
	if (h2.response() != allocator_response::success) return TEST_FAIL;

	size = core::memory::current_memory_usage(subsystem_memory_tag::debug_system);
	if (size < (64 MB)) return TEST_FAIL;
	if (core::memory::total_memory_usage() < (64 MB)) return TEST_FAIL;

	u64 size_snapshot = core::memory::current_memory_usage(subsystem_memory_tag::debug_system);
	core::memory::deallocate(h1);
	size_snapshot = size_snapshot - core::memory::current_memory_usage(subsystem_memory_tag::debug_system);

	if (size_snapshot != (32 MB)) return TEST_FAIL;

	size_snapshot = core::memory::current_memory_usage(subsystem_memory_tag::debug_system);
	core::memory::deallocate(h2);
	size_snapshot = size_snapshot - core::memory::current_memory_usage(subsystem_memory_tag::debug_system);

	if (size_snapshot != (32 MB)) return TEST_FAIL;

	u64 current_size_snapshot = core::memory::total_memory_usage();
	if(total_size_snapshot != current_size_snapshot) return TEST_FAIL;


	return TEST_PASS;
}


bool global_allocator_simple_behavior_test_on_st_2() NOEXP {

	memory_handle_g handles[12]  = { memory_handle_g{} };
	u64 total_size_snapshot = core::memory::total_memory_usage();

	// allocate memory
	for (u8 i = 0; i < 12; i++) {
		handles[i] = core::memory::allocate(
			memory_request_g{
				.size = 8 MB,
				.tag  = (subsystem_memory_tag)i,
			}
		);
	}

	u64 peak_size = core::memory::peak_memory_usage();
	if (peak_size < (12 * 8 MB)) return TEST_FAIL;

	total_size_snapshot = core::memory::total_memory_usage() - total_size_snapshot;
	if(total_size_snapshot != (12 * 8 MB)) return TEST_FAIL;
	
	if(core::memory::peak_memory_usage() < (12 * 8 MB)) return TEST_FAIL;

	u64 snap_sizeshot = core::memory::current_memory_usage(subsystem_memory_tag::unkown);
	core::memory::deallocate(handles[0]);
	snap_sizeshot = snap_sizeshot - core::memory::current_memory_usage(subsystem_memory_tag::unkown);

	if (snap_sizeshot != (8 MB)) return TEST_FAIL;

	// deallocate memory
	for (u8 i = 1; i < 12; i++) {

		snap_sizeshot = core::memory::current_memory_usage(handles[i].tag());
		core::memory::deallocate(handles[i]);
		snap_sizeshot = snap_sizeshot - core::memory::current_memory_usage(handles[i].tag());

		if (snap_sizeshot != (8 MB)) return TEST_FAIL;
	}

	return TEST_PASS;
}

bool global_allocator_allocate2_simple_behavior_test_on_st() NOEXP {

	same_pair<memory_handle_g> h1 = core::memory::allocate_tow(
		memory_request_g{ .size = 32 MB , .tag = subsystem_memory_tag::debug_system },
		memory_request_g{ .size = 32 MB , .tag = subsystem_memory_tag::debug_system }
	);

	if(h1.first.response() != h1.second.response()) return TEST_FAIL;

	if(core::memory::current_memory_usage(subsystem_memory_tag::debug_system) < (64 MB)) return TEST_FAIL;
	if(core::memory::total_memory_usage() < (64 MB)) return TEST_FAIL;
	if(core::memory::peak_memory_usage() < (64 MB)) return TEST_FAIL;

	u64 size_snapshot = core::memory::current_memory_usage(subsystem_memory_tag::debug_system);
	core::memory::deallocate(h1.second);
	size_snapshot = size_snapshot - core::memory::current_memory_usage(subsystem_memory_tag::debug_system);

	if(size_snapshot != (32 MB)) return TEST_FAIL;
	
	size_snapshot = core::memory::current_memory_usage(subsystem_memory_tag::debug_system);
	core::memory::deallocate(h1.first);
	size_snapshot = size_snapshot - core::memory::current_memory_usage(subsystem_memory_tag::debug_system);

	if(size_snapshot != (32 MB)) return TEST_FAIL;

	return TEST_PASS;
}

// just a function to fill memory with garbage value
void write_to_memory(byte* s, byte* e) NOEXP {
	if (e < s) {
		byte* t = s;
		s = e;
		e = t;
	}

	const byte i = 0xFF;
	while (s < e) {
		*s = i;
		 s++;
	}
}

#endif