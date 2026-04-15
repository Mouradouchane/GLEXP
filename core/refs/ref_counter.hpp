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
		std::atomic<u32> count = 0;

		refcounter(const refcounter&) = delete;
		refcounter& operator=(const refcounter&) = delete;

	public :
		// constructor's
		refcounter() = default;
		DLL_API refcounter(u32 _count_ = 0);
		
		// destructor
		virtual ~refcounter();

		// refcounter functions
		DLL_API inline void add_ref()     NOEXP;
		DLL_API inline void release_ref() NOEXP;

		// debug only functions
	#ifdef DEBUG
		DLL_API inline u32 get_ref_count() NOEXP;
		DLL_API inline u32 get_ref_count() const NOEXP;
	#endif

}; 

#endif