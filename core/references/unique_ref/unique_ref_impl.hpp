#pragma once 

#ifndef CORE_UNIQUE_REF_IMPL_HPP
#define CORE_UNIQUE_REF_IMPL_HPP

#include "unique_ref.hpp"
#include "core/logger/logger.hpp"

#ifdef DEBUG
	static inline auto unique_ref_logger = CORE_GET_LOGGER(REFS_LOGGER);
#else 
	static inline auto unique_ref_logger = nullptr;
#endif

/*
	constructor's
*/

template<typename type> unique_ref<type>::unique_ref(type* pointer) NOEXP {
#ifdef DEBUG
	if (pointer == nullptr) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "nullptr passed to unique_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE_HPP(unique_ref_logger);
	}
#endif

	this->memory = pointer;
}


template<typename type> unique_ref<type>::unique_ref(unique_ref<type>&& other) NOEXP {
#ifdef DEBUG
	if (other.memory == nullptr) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "nullptr passed to unique_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE_HPP(unique_ref_logger);
	}
#endif

	this->memory = other.memory;
	other.memory = nullptr;
}

/*
template<typename type> unique_ref<type>::unique_ref(unique_ref<type>& other) NOEXP {
#ifdef DEBUG
	if (other.memory == nullptr) {
		CORE_WARN_HPP(unique_ref_logger, !other.memory, CORE_LOG_CONFIG_ALL, "nullptr passed to unique_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE_HPP(unique_ref_logger);
	}
#endif

	this->memory = other.memory;
	other.memory = nullptr;
}
*/

/*
			destructor
*/
template<typename type>
unique_ref<type>::~unique_ref() NOEXP {

	if (this->memory) {
		this->memory->~type();
		this->memory = nullptr;

		return;
	}

#ifdef DEBUG
	CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "unique_ref<{}> pointer was nullptr in destruction time !", type_name);
#endif
}


/*
			operator's
*/

template<typename type> 
unique_ref<type>& unique_ref<type>::operator= (type* pointer) NOEXP {

	if (pointer == nullptr) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "nullptr passed to unique_ref<{}> using 'operator =' !", type_name);
		return *this;
	}

	if (this->memory == pointer) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "same pointer passed to unique_ref<{}> using 'operator =' !", type_name);
		return *this;
	}

	if (this->memory) {
		this->memory->~type();
	}
	
	this->memory = pointer;

	return *this;
}

template<typename type> 
unique_ref<type>& unique_ref<type>::operator= (unique_ref<type>&& other) NOEXP {

	if (other.memory == nullptr) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "nullptr passed to unique_ref<{}> using 'operator =' !", type_name);
		return *this;
	}

	if (this->memory == other.memory) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL , "self-assignement attempt to unique_ref<{}> using 'operator =' !" , type_name);
		return *this;
	}

	if (this->memory) {
		this->memory->~type();
	}

	this->memory = other.memory;
	other.memory = nullptr;
	
	return *this;
}


/*
template<typename type>
unique_ref<type>& unique_ref<type>::operator= (unique_ref<type>& other) NOEXP {
#ifdef DEBUG
	if (other.memory == nullptr) {
		CORE_WARN_HPP(unique_ref_logger, !other.memory, CORE_LOG_CONFIG_ALL, "nullptr passed to unique_ref<{}> during construction time !", type_name);
	}
#endif

	if (this == &other) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "self-assignement attempt to unique_ref<{}> !", type_name);
		return *this;
	}

	if (this->memory) {
		this->memory->~type();
	}

	this->memory = other.memory;
	other.memory = nullptr;

	return *this;
}
*/

template<typename type> type* unique_ref<type>::operator->() NOEXP {
#ifdef DEBUG
	// note: this will trigger 'debugger' in 'debug-build'
	if (this->memory == nullptr) {
		CORE_FATAL_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL , "attempt to access nullptr unique_ref<{}> !", type_name);
	}
#endif

	return this->memory;
}

template<typename type> type& unique_ref<type>::operator*() NOEXP {

#ifdef DEBUG  // note: this will trigger 'debugger' in 'debug-build'
	if (this->memory == nullptr) {
		CORE_FATAL_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "attempt to dereference nullptr unique_ref<{}> !" , type_name);
	}
#endif

	return *this->memory;
}

template<typename type> const type& unique_ref<type>::operator*() const NOEXP {

#ifdef DEBUG  // this will trigger debugger in debug build
	if (this->memory == nullptr) {
		CORE_FATAL_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "attempt to dereference nullptr unique_ref<{}> !", type_name);
	}
#endif

	return *this->memory;
}


template<typename type> unique_ref<type>::operator bool() const NOEXP {
	return (this->memory != nullptr);
}

/*
				function's
*/

template<typename type>
type* unique_ref<type>::pass_ownership() NOEXP {

	type* ptr = this->memory;
	this->memory = nullptr;
	return ptr;
}


template<typename type> 
bool unique_ref<type>::move_ownership(unique_ref<type>& ref, unique_ref<type>& new_owner) {

	if (ref.memory == nullptr) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "moving-ownership 'nullptr' unique_ref is not allowed!");
		return false;
	}

	if (ref.memory == new_owner.memory || &ref == &new_owner) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "moving-ownership from unique_ref to it's self is probablly caused by a bug !");
		return false;
	}

	if (new_owner.memory) {
		new_owner.memory->~type();
	}

	new_owner.memory = ref.memory;
	ref.memory = nullptr;

	return true;
}

template<core::memory_allocator& allocator, typename... Args>
unique_ref<type> make_unique_ref(Args&&... args) {
	// This assumes your custom allocation happens inside the type's constructor
	// or via a specific 'new' overload.
	return unique_ref<type>(new type(std::forward<Args>(args)...));
}

#endif