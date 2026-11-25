#pragma once

#ifndef GLOBAL_ALLOCATOR_BEHAVIOR_TEST_ON_ST_CPP
#define GLOBAL_ALLOCATOR_BEHAVIOR_TEST_ON_ST_CPP

#include "tools/tester/macros.hpp"

#include "core/memory/global_allocator/global_allocator.hpp"

#include "gloabl_allocator_behavior_test_on_st.hpp"

void write_to_memory(byte* s, byte* e) NOEXP;

bool global_allocator_simple_behavior_test_on_st_1() NOEXP {

	u64 total_size_snapshot = core::global_allocator::allocated_memory();

	memory_handle_g h1 = core::global_allocator::allocate(
		memory_request_g{ 
			.size = 32 MB , 
			.tag = subsystem_memory_tag::debug_system
		}
	);

	if (h1.pointer() == nullptr) return TEST_FAIL;
	if (h1.response() != allocator_response::success) return TEST_FAIL;

	u64 size = core::global_allocator::allocated_memory(subsystem_memory_tag::debug_system);
	if (size < (32 MB)) return TEST_FAIL;

	memory_handle_g h2 = core::global_allocator::allocate(
		memory_request_g{
			.size = 32 MB ,
			.tag = subsystem_memory_tag::debug_system
		}
	);

	if (h2.pointer()  == nullptr) return TEST_FAIL;
	if (h2.response() != allocator_response::success) return TEST_FAIL;

	size = core::global_allocator::allocated_memory(subsystem_memory_tag::debug_system);
	if (size < (64 MB)) return TEST_FAIL;
	if (core::global_allocator::allocated_memory() < (64 MB)) return TEST_FAIL;

	u64 size_snapshot = core::global_allocator::allocated_memory(subsystem_memory_tag::debug_system);
	core::global_allocator::deallocate(h1);
	size_snapshot = size_snapshot - core::global_allocator::allocated_memory(subsystem_memory_tag::debug_system);

	if (size_snapshot != (32 MB)) return TEST_FAIL;

	size_snapshot = core::global_allocator::allocated_memory(subsystem_memory_tag::debug_system);
	core::global_allocator::deallocate(h2);
	size_snapshot = size_snapshot - core::global_allocator::allocated_memory(subsystem_memory_tag::debug_system);

	if (size_snapshot != (32 MB)) return TEST_FAIL;

	u64 current_size_snapshot = core::global_allocator::allocated_memory();
	if(total_size_snapshot != current_size_snapshot) return TEST_FAIL;


	return TEST_PASS;
}


bool global_allocator_simple_behavior_test_on_st_2() NOEXP {

	memory_handle_g handles[12]  = { memory_handle_g{} };
	u64 total_size_snapshot = core::global_allocator::allocated_memory();

	// allocate memory
	for (u8 i = 0; i < 12; i++) {
		handles[i] = core::global_allocator::allocate(
			memory_request_g{
				.size = 8 MB,
				.tag  = (subsystem_memory_tag)i,
			}
		);
	}

	u64 peak_size = core::global_allocator::peak_memory_usage();
	if (peak_size < (12 * 8 MB)) return TEST_FAIL;

	total_size_snapshot = core::global_allocator::allocated_memory() - total_size_snapshot;
	if(total_size_snapshot != (12 * 8 MB)) return TEST_FAIL;
	
	if(core::global_allocator::peak_memory_usage() < (12 * 8 MB)) return TEST_FAIL;

	u64 snap_sizeshot = core::global_allocator::allocated_memory(subsystem_memory_tag::unkown);
	core::global_allocator::deallocate(handles[0]);
	snap_sizeshot = snap_sizeshot - core::global_allocator::allocated_memory(subsystem_memory_tag::unkown);

	if (snap_sizeshot != (8 MB)) return TEST_FAIL;

	// deallocate memory
	for (u8 i = 1; i < 12; i++) {

		snap_sizeshot = core::global_allocator::allocated_memory(handles[i].tag());
		core::global_allocator::deallocate(handles[i]);
		snap_sizeshot = snap_sizeshot - core::global_allocator::allocated_memory(handles[i].tag());

		if (snap_sizeshot != (8 MB)) return TEST_FAIL;
	}

	return TEST_PASS;
}

bool global_allocator_allocate2_simple_behavior_test_on_st() NOEXP {

	same_pair<memory_handle_g> h1 = core::global_allocator::allocate_tow(
		memory_request_g{ .size = 32 MB , .tag = subsystem_memory_tag::debug_system },
		memory_request_g{ .size = 32 MB , .tag = subsystem_memory_tag::debug_system }
	);

	if(h1.first.response() != h1.second.response()) return TEST_FAIL;

	if(core::global_allocator::allocated_memory(subsystem_memory_tag::debug_system) < (64 MB)) return TEST_FAIL;
	if(core::global_allocator::allocated_memory() < (64 MB)) return TEST_FAIL;
	if(core::global_allocator::peak_memory_usage() < (64 MB)) return TEST_FAIL;

	u64 size_snapshot = core::global_allocator::allocated_memory(subsystem_memory_tag::debug_system);
	core::global_allocator::deallocate(h1.second);
	size_snapshot = size_snapshot - core::global_allocator::allocated_memory(subsystem_memory_tag::debug_system);

	if(size_snapshot != (32 MB)) return TEST_FAIL;
	
	size_snapshot = core::global_allocator::allocated_memory(subsystem_memory_tag::debug_system);
	core::global_allocator::deallocate(h1.first);
	size_snapshot = size_snapshot - core::global_allocator::allocated_memory(subsystem_memory_tag::debug_system);

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