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
#define ADD_SHARED_REF(SHARED_REF_PTR) SHARED_REF_PTR->memory->__strong__.fetch_add(1, std::memory_order_relaxed)
#define SUB_SHARED_REF(SHARED_REF_PTR) SHARED_REF_PTR->memory->__strong__.fetch_sub(1, std::memory_order_relaxed)
// for atomic operations
#define ADD_SHARED_REF_ATOMIC(SHARED_REF_PTR) SHARED_REF_PTR->memory->__strong__.fetch_add(1, std::memory_order_acq_rel)
#define SUB_SHARED_REF_ATOMIC(SHARED_REF_PTR) SHARED_REF_PTR->memory->__strong__.fetch_sub(1, std::memory_order_acq_rel)

// for weak_ref
#define ADD_WEAK_REF(WEAK_REF_PTR) WEAK_REF_PTR->memory->__weak__.fetch_add(1, std::memory_order_relaxed)
#define SUB_WEAK_REF(WEAK_REF_PTR) WEAK_REF_PTR->memory->__weak__.fetch_sub(1, std::memory_order_relaxed)
// for atomic operations
#define ADD_WEAK_REF_ATOMIC(WEAK_REF_PTR) WEAK_REF_PTR->memory->__weak__.fetch_add(1, std::memory_order_acq_rel)
#define SUB_WEAK_REF_ATOMIC(WEAK_REF_PTR) WEAK_REF_PTR->memory->__weak__.fetch_sub(1, std::memory_order_acq_rel)


// inject this macro at the end of your class !
// to make it acceptable by "ref counter classes"
#define INJECT_REF_COUNTER(__TYPE__) \
		protected : \
			template<refcounted_type __TYPE__> friend class shared_ref; \
			template<refcounted_type __TYPE__> friend class weak_ref; \
			std::atomic<u32> __strong__(0); \
			std::atomic<u32>  __weak__(0); \
		public : \
			using __is_refcounter_type__ = void;


// few "rules and demands" here for any types we gonna be used with refs 
template<typename type, core::memory::management plan> 
concept refcounted_type = requires(type t) {

	// rule 1 : inject this macro in your type/class --> INJECT_REF_COUNTER(type)
	// why ? that macro add counter and make your type "compatible & usable & managable" by our refs classes.
	typename type::__is_refcounter_type__;

	// rule 2 : your type must have destructor "~type() NOEXP" 
	requires std::destructible<type>;
	requires std::is_nothrow_destructible_v<type>;

	// rule 3 : [mandatory if] your type is self mananging his memory internaly
	requires (plan == core::memory::management::by_others || requires {
		{ t.allocator } -> std::same_as<core::memory_allocator*>;
	});

	// rule 4 [optional] : make destructor of your type virtual , if you have any inheritance !
	// to make sure the right destructor begin called not the wrong one !
	// requires std::has_virtual_destructor_v<type>;

};


/*
	refernce memory management options
	for more infor check : core::memory::management at core/memory/memory.hpp
*/

#define REF_REQUIRE_MANAGEMENT_BY_IT_SELF requires(plan == core::memory::management::by_self)
#define REF_REQUIRE_MANAGEMENT_BY_OTHERS  requires(plan == core::memory::management::by_others)

template<core::memory::management plan>
struct memory_management_plan {
	core::memory_allocator* allocator = nullptr;
	memory_management_plan(core::memory_allocator const& _allocator_) : allocator((core::memory_allocator*)&_allocator_) {}
	memory_management_plan() = default;
};

template<>
struct memory_management_plan<core::memory::management::by_self> {
	memory_management_plan(core::memory_allocator const& _allocator_) {}
	memory_management_plan() = default;
};


// check if cpu support atomic
COMPILE_TIME_ASSERT(!std::atomic<u32>::is_always_lock_free, "your cpu is not supporting atomic's cpu instruction !");

#endif