#pragma once 

#ifndef CORE_SCOPE_LOCK_HPP
#define CORE_SCOPE_LOCK_HPP

#include "core/locks/atomic_lock/atomic_lock.hpp"

namespace core {

	// this used to lock a atomic_lock on scope level and unlock automatically in end of scope .
	// note: this use wait_for_lock which mean 
	DLL_API_CLASS atomic_scope_lock {

	private:
		core::atomic_lock* _lock_;
		id32 id;

	public:
		// constructor
		atomic_scope_lock(core::atomic_lock const& target_lock) NOEXP;

		// destructor
		~atomic_scope_lock() NOEXP;

	private:
		// not allowed constructor's 
		atomic_scope_lock(atomic_scope_lock & other)      = delete;
		atomic_scope_lock(atomic_scope_lock && other)     = delete;
		atomic_scope_lock(atomic_scope_lock const& other) = delete;

		// not allowed operator's
		atomic_scope_lock& operator=(atomic_scope_lock & other)      = delete;
		atomic_scope_lock& operator=(atomic_scope_lock && other)     = delete;
		atomic_scope_lock& operator=(atomic_scope_lock const& other) = delete;

	};
	// class atomic_scope_lock end


}
// namespace core end


#endif