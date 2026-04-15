#pragma once 

#ifndef CORE_REFCOUNTER_CPP
#define CORE_REFCOUNTER_CPP

#include "ref_counter.hpp"


DLL_API_CLASS refcounter {
	protected :
		std::atomic<u32> __count__ = 0;

		refcounter(refcounter const&) = delete;
		refcounter& operator=(refcounter const&) = delete;

	public :
		// constructor's
		refcounter() = default;
		DLL_API refcounter(u32 _count_ = 0) : __count__(_count_) {

		}
		
		// destructor
		virtual ~refcounter() = default;

		// refcounter functions
		DLL_API inline void add_ref() NOEXP {
			__count__ += 1;
		}

		DLL_API inline void release_ref() NOEXP {
			if (__count__) __count__ -= 1;
		}

		// debug only functions
	#ifdef DEBUG
		DLL_API inline u32 get_ref_count() NOEXP {
			return __count__;
		}
		DLL_API inline u32 get_ref_count() const NOEXP {
			return __count__;
		}
	#endif

}; 

#endif