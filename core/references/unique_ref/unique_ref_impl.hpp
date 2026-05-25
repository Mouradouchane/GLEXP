#pragma once 

#ifndef CORE_UNIQUE_REF_IMPL_HPP
#define CORE_UNIQUE_REF_IMPL_HPP

#include "core/logger/logger.hpp"
#include "unique_ref.hpp"

#ifdef DEBUG
	static inline auto unique_ref_logger = CORE_GET_LOGGER(REFS_LOGGER);
#else 
	static inline auto unique_ref_logger = nullptr;
#endif

/*
	constructor's
*/

UNIQUE_REF_TEMPLATE
unique_ref<type>::unique_ref(core::memory_allocator* allocator, type* memory_ptr) NOEXP {

	this->allocator = allocator;
	this->memory    = memory_ptr;
}

UNIQUE_REF_TEMPLATE
template<typename... parameters>
unique_ref<type>::unique_ref(core::memory_allocator const& allocator, parameters&&... constructor_parameters) NOEXP {

	this->allocator = (core::memory_allocator*)&allocator;
	this->memory    = (type*)this->allocator->allocate(sizeof(type));

	this->memory = new (this->memory) type(std::forward<parameters>(constructor_parameters)...);
}

UNIQUE_REF_TEMPLATE 
unique_ref<type>::unique_ref(unique_ref<type>&& unique_reference) NOEXP {

	if (unique_reference.memory && unique_reference.allocator) {
		this->allocator = unique_reference.allocator;
		this->memory    = unique_reference.memory;

		unique_reference.allocator = nullptr;
		unique_reference.memory    = nullptr;
	}
	else {
		CORE_FATAL_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, REF_INVALID, "unique_ref memory-block or allocator is nullptr");
	}
}

UNIQUE_REF_TEMPLATE
unique_ref<type>::unique_ref(unique_ref<type>& unique_reference) NOEXP {

	if (unique_reference.memory && unique_reference.allocator) {
		this->allocator = unique_reference.allocator;
		this->memory = unique_reference.memory;

		unique_reference.allocator = nullptr;
		unique_reference.memory    = nullptr;
	}
	else {
		CORE_FATAL_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, REF_INVALID, "unique_ref memory-block or allocator is nullptr");
	}
}

/*
			destructor
*/

UNIQUE_REF_TEMPLATE
unique_ref<type>::~unique_ref() NOEXP {

	this->memory->~type();
	this->allocator->deallocate(this->memory);

	this->allocator = nullptr;
	this->memory    = nullptr;

	CORE_DEBUG_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "unique_ref<{}> destructor called");
}


/*
			operator's
*/

UNIQUE_REF_TEMPLATE 
unique_ref<type>& unique_ref<type>::operator= (unique_ref<type>&& unique_reference) NOEXP {

	if (this->memory == unique_reference.memory) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, REF_SELF_ASSIGN);
		return *this;
	}

	if (unique_reference.memory && unique_reference.allocator) {
		this->deal_with_current_reference();

		this->allocator = unique_reference.allocator;
		this->memory    = unique_reference.memory;

		unique_reference.allocator = nullptr;
		unique_reference.memory    = nullptr;
	}

	return *this;
}


UNIQUE_REF_TEMPLATE 
type* unique_ref<type>::operator->() NOEXP {
#ifdef DEBUG
	if (this->memory == nullptr) {
		CORE_FATAL_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL , "attempt to access nullptr unique_ref<{}> using ->", type_name);
	}
#endif

	return (this->memory) ? this->memory : &this->__dummy__;
}

UNIQUE_REF_TEMPLATE 
type& unique_ref<type>::operator*() NOEXP {

#ifdef DEBUG
	if (this->memory == nullptr) {
		CORE_FATAL_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "attempt to dereference nullptr unique_ref<{}> using *" , type_name);
	}
#endif

	return (this->memory) ? *this->memory : &this->__dummy__;
}

UNIQUE_REF_TEMPLATE 
const type& unique_ref<type>::operator*() const NOEXP {

#ifdef DEBUG
	if (this->memory == nullptr) {
		CORE_FATAL_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "attempt to dereference nullptr unique_ref<{}> using *", type_name);
	}
#endif

	return (this->memory) ? (const type&)*this->memory : &this->__dummy__;
}


UNIQUE_REF_TEMPLATE 
unique_ref<type>::operator bool() const NOEXP {
	return (this->memory != nullptr);
}

/*
				function's
*/

UNIQUE_REF_TEMPLATE
pair_tow_pointers<core::memory_allocator, type> unique_ref<type>::pass_ownership() NOEXP {

	type* mem = this->memory;
	core::memory_allocator* alloc = this->allocator;

	this->memory    = nullptr;
	this->allocator = nullptr;

	return pair_tow_pointers<core::memory_allocator , type> { mem , alloc };
}


UNIQUE_REF_TEMPLATE 
bool unique_ref<type>::move_ownership(unique_ref<type>& ref, unique_ref<type>& new_owner) {

	if (&ref == &new_owner) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, REF_SELF_ASSIGN);
		return false;
	}

	// clean for new reference
	this->deal_with_current_reference();

	// pass ownership
	new_owner.allocator = ref.allocator;
	new_owner.memory    = ref.memory;

	ref.allocator = nullptr;
	ref.memory    = nullptr;

	return true;
}


#endif