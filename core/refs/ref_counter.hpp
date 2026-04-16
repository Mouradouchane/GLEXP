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

		// public functions
		INLINE void     add_strong_ref() NOEXP;
		INLINE void release_strong_ref() NOEXP;

		INLINE void     add_weak_ref()   NOEXP;
		INLINE void release_weak_ref()   NOEXP;

		// debug-only functions
	#ifdef DEBUG
		INLINE u32 get_strong_count() NOEXP;
		INLINE u32 get_strong_count() const NOEXP;

		INLINE u32 get_weak_count() NOEXP;
		INLINE u32 get_weak_count() const NOEXP;
	#endif

}; 
// class refcounter end

#endif