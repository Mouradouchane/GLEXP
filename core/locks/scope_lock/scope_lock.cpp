#pragma once 

#ifndef CORE_SCOPE_LOCK_CPP
#define CORE_SCOPE_LOCK_CPP

#include "scope_lock.hpp"
#include "core/logger/logger.hpp"

#ifdef DEBUG
	static auto _core_scoped_lock_logger_ = CORE_GET_LOGGER(WORK_SYSTEM_LOGGER);
#else 
	static auto _core_scoped_lock_logger_ = nullptr;
#endif

#define _LOGGER_  _core_scoped_lock_logger_   

#define CORE_SCOPE_LOCK_FAILED_LOCK   "scope_lock failed to lock atomic_lock={} , id={} !"
#define CORE_SCOPE_LOCK_FAILED_UNLOCK "scope_lock attempt to unlock a lock he doesn't own in end of scope , id={} !"

namespace core {

	/*
		constructor
	*/
	atomic_scope_lock::atomic_scope_lock(core::atomic_lock const& target_lock) NOEXP {
		
		// get lock
		_lock_ = (core::atomic_lock*)&target_lock;
		
		// own the lock
		_id_ = _lock_->wait_for_lock();

	#ifdef DEBUG
		CORE_ERROR_IF(
			_id_ == core::atomic_lock::invalid_id, CORE_LOG_CONFIG_ALL, CORE_SCOPE_LOCK_FAILED_LOCK ,
			core::pointer_to_hex_string(_lock_) , _id_
		);
	#endif
	}

	/*
		destructor
	*/
	atomic_scope_lock::~atomic_scope_lock() NOEXP {

	#ifdef DEBUG
		bool result = _lock_->release(_id_);
		CORE_ERROR_IF(!result, CORE_LOG_CONFIG_ALL, CORE_SCOPE_LOCK_FAILED_UNLOCK, _id_);
	#else
		// give back the lock
		_lock_->unlock(_id_);
	#endif

		_lock_ = nullptr;
		_id_   = 0;
	}


} // namesapce core end

#endif