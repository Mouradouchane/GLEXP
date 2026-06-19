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
		this->_lock_ = (core::atomic_lock*)&target_lock;
		
		// try until its locked
		this->id = this->_lock_->wait_for_lock();

	#ifdef DEBUG
		CORE_FATAL_IF(
			this->id == core::atomic_lock::invalid_id, CORE_LOG_CONFIG_ALL, CORE_SCOPE_LOCK_FAILED_LOCK ,
			core::pointer_to_hex_string(this->_lock_) , this->id
		);
	#endif
	}

	/*
		destructor
	*/
	atomic_scope_lock::~atomic_scope_lock() NOEXP {

	#ifdef DEBUG
		bool result = this->_lock_->release(this->id);
		CORE_ERROR_IF(!result, CORE_LOG_CONFIG_ALL , CORE_SCOPE_LOCK_FAILED_UNLOCK, this->id);
	#else
		this->_lock_->unlock(this->id);
	#endif

		this->_lock_ = nullptr;
		this->id   = 0;
	}


} // namesapce core end

#endif