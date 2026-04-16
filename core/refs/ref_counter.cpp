#pragma once 

#ifndef CORE_REFCOUNTER_CPP
#define CORE_REFCOUNTER_CPP

#include "core/logger/logger.hpp"
#include "ref_counter.hpp"

CORE_GET_LOGGER_VAR(_ref_ctr_logger_, REFS_LOGGER);
#define _LOGGER_ _ref_ctr_logger_

/*
	constructor
*/
refcounter::refcounter(u32 strong_counter = 0, u32 weak_counter = 0)
	: __strong__(strong_counter), __weak__(weak_counter) {

};

// refcounter functions
INLINE void refcounter::add_strong_ref() NOEXP {
	this->__strong__ += 1;
}

INLINE void refcounter::release_strong_ref() NOEXP {
	if (this->__strong__ ) this->__strong__ -= 1;
#ifdef DEBUG
	else { 
		// todo: define errors for refs + log
		CORE_ERROR("");
		DebugBreak;
	}
#endif
}

INLINE void refcounter::add_weak_ref() NOEXP {
	this->__weak__ += 1;
}

INLINE void refcounter::release_weak_ref() NOEXP {
	if (this->__weak__) this->__weak__ -= 1;
#ifdef DEBUG
	else {
		// todo: define warnings for refs + log
		CORE_WARN("some weak_ref double release , probably caused by a bug !");
	}
#endif
}

/*
	debug-only functions
*/
#ifdef DEBUG
	INLINE u32 refcounter::get_strong_count() NOEXP {
		return this->__strong__;
	}
	INLINE u32 refcounter::get_strong_count() const NOEXP {
		return this->__strong__;
	}

	INLINE u32 refcounter::get_weak_count() NOEXP {
		return this->__weak__;
	}
	INLINE u32 refcounter::get_weak_count() const NOEXP {
		return this->__weak__;
	}
#endif

#endif