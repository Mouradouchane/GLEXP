#pragma once 

#ifndef CORE_REFCOUNTER_HPP
#define CORE_REFCOUNTER_HPP

#include <type_traits>
#include <atomic>

#include "core/macros.hpp"
#include "core/assert.hpp"
#include "core/types.hpp"

		
/*
	note: refcounter class used as "intrusive refernce counter" !
	      used for management objects life-time automatically
*/
DLL_API_CLASS refcounter {

	// who can access refcounter class 
	template<typename type> friend class ref;
	template<typename type> friend class weak_ref;

	protected :
		// counters
		std::atomic<u32> __strong__ = 0; 
		std::atomic<u32>  __weak__  = 0;

		// note: copying counter not allowed
		refcounter(refcounter const&) = delete;
		refcounter& operator=(refcounter const&) = delete;

	public :
		// constructor's
		refcounter() = default;
		refcounter(u32 strong_counter = 0 , u32 weak_counter = 0);
		
		// destructor
		virtual ~refcounter();
}; 
// class refcounter end

#endif