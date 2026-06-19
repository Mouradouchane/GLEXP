#pragma once 

#ifndef CORE_ATOMIC_LOCK_CPP
#define CORE_ATOMIC_LOCK_CPP

#include "core/macros.hpp"
#include "core/assert.hpp"
#include "core/logger/logger.hpp"

#include "atomic_lock.hpp"

#ifdef DEBUG
	static auto _core_atomic_locks_logger_ = CORE_GET_LOGGER(WORK_SYSTEM_LOGGER);
#else 
	static auto _core_atomic_locks_logger_ = nullptr;
#endif

#define _LOGGER_  _core_atomic_locks_logger_

#define CORE_ATOMIC_UAF_BUG "core::atomic_lock 'use after free' bug detected !"

/*
	macro for making CPU core "sleep/skip" few cycles
*/
#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
	// for x86 x64
	#include <immintrin.h>
	#define SLEEP_FEW_CYCLES() _mm_pause()
#elif defined(__ARM_ARCH) || defined(__aarch64__)
	// for ARM 
	#define SLEEP_FEW_CYCLES() __asm__ __volatile__("yield" ::: "memory")
#else
	COMPILE_TIME_ASSERT("unsupported cpu arch !");
#endif

namespace core {

/*
	 constructor
*/
atomic_lock::atomic_lock() NOEXP 
	:_lock_({0 , core::lock_status::free})
{
	
}

/*
	destructor
*/
atomic_lock::~atomic_lock() NOEXP {
	core::lock_object current_state = this->_lock_.load(ATOMIC_RELAXED_ORDER);

#ifdef DEBUG
	if (current_state.status == CORE_LOCK_BUSY) {
		CORE_FATAL(CORE_LOG_CONFIG_ALL, CORE_ATOMIC_UAF_BUG);
	}
#endif

	// memory-poisoning with unsuable garbage data
	this->_lock_.store({ 0xFFFFFFFF , (core::lock_status)0xFFFFFFFF } , ATOMIC_RELAXED_ORDER);

}

/*
	public functions
*/

id32 atomic_lock::try_lock() NOEXP {

	core::lock_object expected_lock{ 0 , CORE_LOCK_FREE };

	const core::lock_object new_lock{ this->unique_ownership_id.load() , CORE_LOCK_BUSY };

	// try to lock
	if( this->_lock_.compare_exchange_strong(expected_lock, new_lock, ATOMIC_ACQUIRE_ORDER, ATOMIC_RELAXED_ORDER) ) {

		// if we own the lock successfuly
		this->unique_ownership_id.fetch_add(1, ATOMIC_RELAXED_ORDER);
		return (id32)this->unique_ownership_id.load();
	}
	// else failed to lock
	return atomic_lock::invalid_id;
}

// todo[optimize] : try to optimize with "branch-less" later if possible
id32 atomic_lock::wait_for_lock() NOEXP {
	
	/*
		note[important]: 
			- maybe this is to much !!!!because each sleep could cost about "150 cpu cycle"
		    - so 32*150 that's a 4800 cycle wasted waiting for lock    
	*/
	u8 max_attempts = 32; 
	
	id32 id = atomic_lock::invalid_id;

	while (max_attempts) {
		id = this->try_lock();

		if (id) return id;
		
		max_attempts -= 1;

		SLEEP_FEW_CYCLES();
	}

	/*
		cpu firendly waiting for lock , but it will take more time to sleep by simply giving cpu to other threads .
	*/
	while (true) {
		id = this->try_lock();

		if (id) return id;

		std::this_thread::yield();
	}

}

bool atomic_lock::release(id32 ownership_id) NOEXP {

	core::lock_object new_lock{ 0 , CORE_LOCK_FREE };
	core::lock_object expected_lock{ ownership_id , CORE_LOCK_BUSY };

	// try to unlock / give back ownership
	// if ( this->_lock_.compare_exchange_strong(expected_lock, new_lock , ATOMIC_ACQUIRE_ORDER, ATOMIC_RELAXED_ORDER) ) {
	if (this->_lock_.compare_exchange_strong(expected_lock, new_lock, ATOMIC_RELEASE_ORDER, ATOMIC_RELAXED_ORDER)) {
		this->unique_ownership_id.fetch_add(1);
		return true;
	}
	// else means invalid ownership id passed
	return false;
}

bool atomic_lock::is_locked() NOEXP {
	return (bool)this->_lock_.load(ATOMIC_RELEASE_ORDER).status;
}

// not: this function just for spdlog and std to use them
DONT_USE INLINE void atomic_lock::lock() NOEXP {
/*
	this->wait_for_lock();
*/
}

// not: this function just for spdlog and std to use them
DONT_USE INLINE void atomic_lock::unlock() NOEXP {
/*
	core::lock_object current = this->_lock_.load(ATOMIC_RELAXED_ORDER);

	if (current.status == CORE_LOCK_BUSY) {
		this->release(current.owner_id); // Safely authenticates and releases
	}
*/
}

} // namespace core end

#endif