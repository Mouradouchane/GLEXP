#pragma once 

#ifndef CORE_REFCOUNTER_HPP
#define CORE_REFCOUNTER_HPP

#include <concepts>
#include <atomic>

#include "core/types.hpp"
#include "core/assert.hpp"
#include "core/memory/memory.hpp"

/*
	few macros to inc/dec refernce's counters
*/ 

// for shared_ref
#define ADD_SHARED_REF(SHARED_REF_PTR) SHARED_REF_PTR->ctr->__strong__.fetch_add(1, std::memory_order_relaxed)
#define SUB_SHARED_REF(SHARED_REF_PTR) SHARED_REF_PTR->ctr->__strong__.fetch_sub(1, std::memory_order_relaxed)
// for atomic operations
#define ADD_SHARED_REF_ATOMIC(SHARED_REF_PTR) SHARED_REF_PTR->ctr->__strong__.fetch_add(1, std::memory_order_acq_rel)
#define SUB_SHARED_REF_ATOMIC(SHARED_REF_PTR) SHARED_REF_PTR->ctr->__strong__.fetch_sub(1, std::memory_order_acq_rel)

// for weak_ref
#define ADD_WEAK_REF(WEAK_REF_PTR) WEAK_REF_PTR->ctr->__weak__.fetch_add(1, std::memory_order_relaxed)
#define SUB_WEAK_REF(WEAK_REF_PTR) WEAK_REF_PTR->ctr->__weak__.fetch_sub(1, std::memory_order_relaxed)
// for atomic operations
#define ADD_WEAK_REF_ATOMIC(WEAK_REF_PTR) WEAK_REF_PTR->ctr->__weak__.fetch_add(1, std::memory_order_acq_rel)
#define SUB_WEAK_REF_ATOMIC(WEAK_REF_PTR) WEAK_REF_PTR->ctr->__weak__.fetch_sub(1, std::memory_order_acq_rel)

/*
	macros of errors and warnings
*/
#define REF_INVALID        "reference invalid because of {}"
#define REF_INVALID_CTR    "reference counter-block is invalid"
#define REF_INVALID_MEMORY "reference memory is invalid"
#define REF_SELF_ASSIGN    "reference self assignement detected"
#define REF_IS_DEAD        "attempt to access dead reference"

/*
	reference counter-block
*/
struct counter_block {

private:
	std::atomic<u32> __strong__ = 0u;
	std::atomic<u32>  __weak__  = 0u;
	core::memory_allocator* allocator = nullptr;

	template<typename type> friend class shared_ref;
	template<typename type> friend class weak_ref;
};


// check if cpu support atomic
COMPILE_TIME_ASSERT(!std::atomic<u32>::is_always_lock_free, "your cpu is not supporting atomic's cpu instruction !");

#endif