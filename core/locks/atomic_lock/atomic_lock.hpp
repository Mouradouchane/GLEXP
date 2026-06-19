#pragma once 

#ifndef CORE_ATOMIC_LOCK_HPP
#define CORE_ATOMIC_LOCK_HPP

#include <atomic>

#include "core/macros.hpp"
#include "core/types.hpp"

// few macros for usage
#define CORE_LOCK_BUSY core::lock_status::busy
#define CORE_LOCK_FREE core::lock_status::free

#define CORE_FREE_LOCK_OBJECT core::lock_object{ 0 , CORE_LOCK_FREE }
#define CORE_BUSY_LOCK_OBJECT core::lock_object{ 1 , CORE_LOCK_BUSY }

// this macros for ordering and atomic operator
#define ATOMIC_ACQUIRE_ORDER std::memory_order_acquire // lock for read/write + syncing with other core's
#define ATOMIC_RELEASE_ORDER std::memory_order_release // release + syncing with other core's

#define ATOMIC_RELAXED_ORDER std::memory_order_relaxed // no syncing with other's

namespace core {

	enum lock_status : u32 {
		free = 0, // unlocked
		busy = 1, // locked 
	};

	// 64-bit packed struct to avoid "multi-variables" "data-race" problem
	struct alignas(8) lock_object {
		id32        owner_id; // u32
		lock_status status;   // u32

		// for "trivialy-copyable"
		lock_object() = default;
		lock_object(id32 id, lock_status status) : owner_id(id), status(status) {}
		lock_object(const lock_object&) = default;
		lock_object& operator=(const lock_object&) = default;
		
		~lock_object() = default;
	};

	// this is a "owner-authenticated-lock" operating with ownership ids
	// when you lock it you obtain a ownership id32 , only the owner id can unlock it
	DLL_API_CLASS atomic_lock {
	private:
		std::atomic<id32> unique_ownership_id = 1;
		std::atomic<core::lock_object> _lock_;

	public:
		static const id32 invalid_id = 0;

		// constructor
		 atomic_lock() NOEXP;

		// destructor
		~atomic_lock() NOEXP;

		// public functions
		id32 try_lock() NOEXP;
		id32 wait_for_lock() NOEXP;

		bool release(id32 ownership_id) NOEXP;
		bool is_locked() NOEXP;

		// note: - spdlog tries to force some rules to any class who use it and have a std lock like std::atomic !
		//       - this function not working but here just to pass spdlog rules .
		DONT_USE INLINE void lock() NOEXP;
		DONT_USE INLINE void unlock() NOEXP;

	private:
		// not allowed constructor's 
		atomic_lock(atomic_lock& other) = delete;
		atomic_lock(atomic_lock&& other) = delete;
		atomic_lock(atomic_lock const& other) = delete;

		// not allowed operator's
		atomic_lock& operator=(atomic_lock& other) = delete;
		atomic_lock& operator=(atomic_lock&& other) = delete;
		atomic_lock& operator=(atomic_lock const& other) = delete;

	};
	// class atomic_lock end


} // namespace core end

#endif