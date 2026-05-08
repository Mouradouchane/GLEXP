#pragma once 

#ifndef CORE_UNIQUE_REF_IMPL_HPP
#define CORE_UNIQUE_REF_IMPL_HPP

#include "unique_ref.hpp"
#include "core/logger/logger.hpp"


/*
	constructor's
*/

template<typename type> unique_ref<type>::unique_ref(type* pointer) NOEXP {
#ifdef DEBUG
	if (pointer == nullptr) {
		CORE_WARN(!pointer, CORE_LOG_CONFIG_ALL, "nullptr passed to unique_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE();
	}
#endif

	this->memory = pointer;
}

template<typename type> unique_ref<type>::unique_ref(unique_ref<type>& other) NOEXP {
#ifdef DEBUG
	if (other.pointer == nullptr) {
		CORE_WARN(!pointer, CORE_LOG_CONFIG_ALL, "nullptr passed to unique_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE();
	}
#endif

	this->memory = pointer;
	other.memory = nullptr;
}

template<typename type> unique_ref<type>::unique_ref(unique_ref<type>&& other) NOEXP {
#ifdef DEBUG
	if (other.pointer == nullptr) {
		CORE_WARN(!pointer, CORE_LOG_CONFIG_ALL, "nullptr passed to unique_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE();
	}
#endif

	this->memory = pointer;
	other.memory = nullptr;
}

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

	CORE_WARN(CORE_LOG_CONFIG_ALL, "unique_ref<{}> pointer was nullptr in destruction time !");
}


/*
			operator's
*/

template<typename type> 
unique_ref<type>& unique_ref<type>::operator= (type* pointer) NOEXP {
#ifdef DEBUG
	if (pointer == nullptr) {
		CORE_WARN(!pointer, CORE_LOG_CONFIG_ALL, "nullptr passed to unique_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE();
	}
#endif

	this->memory = pointer;

	return *this;
}

template<typename type> 
unique_ref<type>& unique_ref<type>::operator= (unique_ref<type>& other) NOEXP {
#ifdef DEBUG
	if (other.pointer == nullptr) {
		CORE_WARN(!pointer, CORE_LOG_CONFIG_ALL, "nullptr passed to unique_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE();
	}
#endif

	this->memory = pointer;
	other.memory = nullptr;

	return *this;
}

template<typename type> 
unique_ref<type>& unique_ref<type>::operator= (unique_ref<type>&& other) NOEXP {
#ifdef DEBUG
	if (other.pointer == nullptr) {
		CORE_WARN(!pointer, CORE_LOG_CONFIG_ALL, "nullptr passed to unique_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE();
	}
#endif

	this->memory = pointer;
	other.memory = nullptr;

	return *this;
}

template<typename type> type* unique_ref<type>::operator->() NOEXP {
	return this->memory;
}

template<typename type> type& unique_ref<type>::operator*()  NOEXP {
#ifdef DEBUG
	CORE_FATAL_IF(this->memory == nullptr, "attempt to dereference nullptr unique_ref<{}> !" , type_name);
#endif

	return *this->memory;
}

template<typename type> const type& unique_ref<type>::operator*()  const NOEXP {
#ifdef DEBUG
	CORE_FATAL_IF(this->memory == nullptr, "attempt to dereference nullptr unique_ref<{}> !", type_name);
#endif

	return *this->memory;
}


template<typename type> unique_ref<type>::operator bool() const NOEXP {
	return (this->memory != nullptr);
}

#endif