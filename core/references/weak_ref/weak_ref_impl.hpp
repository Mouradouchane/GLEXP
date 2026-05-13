#pragma once
 
#ifndef CORE_WEAK_REF_IMPL_HPP
#define CORE_WEAK_REF_IMPL_HPP

#include "weak_ref.hpp"
#include "core/status/status.hpp"
#include "core/logger/logger.hpp"

#ifdef DEBUG
	static auto weak_ref_logger = CORE_GET_LOGGER(REFS_LOGGER);
#else 
	static auto weak_ref_logger = nullptr;
#endif

#define WEAK_REF_TEMPLATE template<typename type>

/*
			constructor's
*/

WEAK_REF_TEMPLATE	
weak_ref<type>::weak_ref(shared_ref<type> const& reference) NOEXP {

	if (reference.memory != nullptr) {
		this->ctr    = reference.ctr;
		this->memory = reference.memory;

		ADD_WEAK_REF_ATOMIC(this);
	}
#ifdef DEBUG
	else {
		CORE_WARN_HPP(weak_ref_logger,CORE_LOG_CONFIG_ALL,"nullptr memory passed to weak_ref<{}> during construction time !" , type_name);
	}
#endif
}


WEAK_REF_TEMPLATE
weak_ref<type>::weak_ref(weak_ref<type> const& reference) NOEXP {

	if (reference.memory != nullptr) {
		this->ctr    = reference.ctr;
		this->memory = reference.memory;

		ADD_WEAK_REF_ATOMIC(this);
	}
#ifdef DEBUG
	else {
		CORE_WARN_HPP(weak_ref_logger,CORE_LOG_CONFIG_ALL,"nullptr memory passed to weak_ref<{}> during construction time !", type_name);
	}
#endif
}

WEAK_REF_TEMPLATE
weak_ref<type>::weak_ref(weak_ref<type>&& reference) NOEXP {
	
	if (reference.memory != nullptr) {
		this->ctr     = reference.ctr;
		this->memory  = reference.memory;

		reference.ctr      = nullptr;
		reference.memory   = nullptr;
	}
#ifdef DEBUG
	else {
		CORE_WARN_HPP(weak_ref_logger,CORE_LOG_CONFIG_ALL,"nullptr memory passed to weak_ref<{}> during construction time !", type_name);
	}
#endif
}

/*
	destructor
*/
WEAK_REF_TEMPLATE
weak_ref<type>::~weak_ref( ) NOEXP {

	// if no reference is alive
	if ((this->ctr->__strong__ == 0) && (SUB_WEAK_REF_ATOMIC(this) == 0) ) {

		// deallocate counter-block
		this->ctr->allocator->deallocate(this->ctr);

		CORE_DEBUG_HPP(weak_ref_logger,CORE_LOG_CONFIG_ALL,"weak_ptr<{}> deallocating reference counter-block ." , type_name);
	}

}

/*
	upcasting constructor's
*/
#define WEAK_REF_DERIVED_TEMPLATE template<std::derived_from<type> derived_type>

WEAK_REF_TEMPLATE 
WEAK_REF_DERIVED_TEMPLATE
weak_ref<type>::weak_ref(weak_ref<derived_type> const& derived_reference) NOEXP {

	if (derived_reference.ctr != nullptr && derived_reference.memory != nullptr) {
		// disconnect from current ref
		if (this->memory) {
			SUB_WEAK_REF_ATOMIC(this);
		}

		// connect to new ref
		this->ctr    = derived_reference.ctr;
		this->memory = derived_reference.memory;

		ADD_WEAK_REF_ATOMIC(this);
	}
#ifdef DEBUG
	else {
		CORE_WARN_HPP(weak_ref_logger,CORE_LOG_CONFIG_ALL,"nullptr memory passed to weak_ref<{}> during construction time !", type_name);
	}
#endif
}

WEAK_REF_TEMPLATE 
WEAK_REF_DERIVED_TEMPLATE
weak_ref<type>::weak_ref(weak_ref<derived_type>&& derived_reference) NOEXP {

	if (derived_reference.ctr != nullptr && derived_reference.memory != nullptr) {
		// disconnect from current ref
		if (this->memory) {
			SUB_WEAK_REF_ATOMIC(this);
		}

		// move new ref
		this->ctr    = derived_reference.ctr;
		this->memory = derived_reference.memory;

		derived_reference.ctr    = nullptr;
		derived_reference.memory = nullptr;
	}
#ifdef DEBUG
	else {
		CORE_WARN_HPP(weak_ref_logger,CORE_LOG_CONFIG_ALL,"nullptr memory passed to weak_ref<{}> during construction time !", type_name);
	}
#endif
}

WEAK_REF_TEMPLATE 
WEAK_REF_DERIVED_TEMPLATE
weak_ref<type>::weak_ref(shared_ref<derived_type> const& derived_reference) NOEXP {

}


/*
	private constructors
*/ 
WEAK_REF_TEMPLATE
WEAK_REF_DERIVED_TEMPLATE
weak_ref<type>::weak_ref(derived_type* derived_ref_memory) NOEXP {

	this->memory = derived_ref_memory;
	this->ctr    = (counter_block*)(derived_ref_memory + sizeof(type));

	ADD_WEAK_REF_ATOMIC(this);
}

WEAK_REF_TEMPLATE
weak_ref<type>::weak_ref(type* other_ref_memory) NOEXP {

	this->memory = other_ref_memory;
	this->ctr    = (counter_block*)(other_ref_memory + sizeof(type));

	ADD_WEAK_REF_ATOMIC(this);
}

/*
			operators
*/
	
WEAK_REF_TEMPLATE  // copy operator
weak_ref<type>& weak_ref<type>::operator=(weak_ref<type> const& other_reference) NOEXP {

	if (this == &other_reference || this->memory == other_reference.memory) {
		CORE_WARN_HPP(weak_ref_logger,CORE_LOG_CONFIG_ALL,"assign weak_ref<{}> to it self is probablly casued by a bug !" , type_name);
		return *this;
	}

	if (other_reference.memory != nullptr) {
		if (this->memory) SUB_WEAK_REF_ATOMIC(this);

		this->ctr    = other_reference.ctr;
		this->memory = other_reference.memory;
		ADD_WEAK_REF_ATOMIC(this);
	}
#ifdef DEBUG
	else {
		CORE_WARN_HPP(weak_ref_logger, CORE_LOG_CONFIG_ALL, "nullptr memory passed to weak_ref<{}> during construction time !", type_name);
	}
#endif
}

WEAK_REF_TEMPLATE  // copy operator
weak_ref<type>& weak_ref<type>::operator=(shared_ref<type> const& other_reference) NOEXP {

	if (this == &other_reference || this->memory == other_reference.memory) {
		CORE_WARN_HPP(weak_ref_logger, CORE_LOG_CONFIG_ALL, "assign weak_ref<{}> to it self is probablly casued by a bug !", type_name);
		return *this;
	}

	
	if (other_reference.memory != nullptr) {
		if (this->memory) SUB_WEAK_REF_ATOMIC(this);

		this->ctr    = other_reference.ctr;
		this->memory = other_reference.memory;

		ADD_WEAK_REF_ATOMIC(this);
	}
#ifdef DEBUG
	else {
		CORE_WARN_HPP(weak_ref_logger, CORE_LOG_CONFIG_ALL, "nullptr memory passed to weak_ref<{}> during construction time !", type_name);
	}
#endif
}

WEAK_REF_TEMPLATE // move operator
weak_ref<type>& weak_ref<type>::operator=(weak_ref<type>&& other_reference) NOEXP {
	
	if (this == &other_reference || this->memory == other_reference.memory) {
		CORE_WARN_HPP(weak_ref_logger, CORE_LOG_CONFIG_ALL, "assign weak_ref<{}> to it self is probablly casued by a bug !", type_name);
		return *this;
	}

	if (other_reference.memory != nullptr) {

		if (this->memory) SUB_WEAK_REF_ATOMIC(this);

		this->ctr    = other_reference.ctr;
		this->memory = other_reference.memory;

		other_reference.ctr    = nullptr;
		other_reference.memory = nullptr;
	}
#ifdef DEBUG
	else {
		CORE_WARN_HPP(weak_ref_logger,CORE_LOG_CONFIG_ALL,"nullptr memory passed to weak_ref<{}> during construction time !", type_name);
	}
#endif
}

WEAK_REF_TEMPLATE 
type& weak_ref<type>::operator*() NOEXP {

#ifdef DEBUG
	if (this->memory == nullptr) {
		CORE_FATAL_HPP(weak_ref_logger, CORE_LOG_CONFIG_ALL, "attempt to dereference nullptr weak_ref<{}>", type_name);
	}
#endif

	return (this->memory) ? *this->memory : weak_ref<type>::__dummy__;
}

WEAK_REF_TEMPLATE 
const type& weak_ref<type>::operator*() const NOEXP {

#ifdef DEBUG
	if (this->memory == nullptr) {
		CORE_FATAL_HPP(weak_ref_logger, CORE_LOG_CONFIG_ALL, "attempt to dereference nullptr weak_ref<{}>", type_name);
	}
#endif

	return (this->memory) ? *this->memory : weak_ref<type>::__dummy__;
}

WEAK_REF_TEMPLATE 
weak_ref<type>::operator bool() const NOEXP {
	return (this->memory != nullptr);
}

/*
			function's
*/

// note: use this to do downcasting "base --> derived"
// note: don't use dynamic_cast_to a lot to avoid preformance costs
WEAK_REF_TEMPLATE
template<typename family_type>
weak_ref<family_type> weak_ref<type>::dynamic_cast_to() {

	COMPILE_TIME_ASSERT(
		!std::is_base_of_v<type family_type> && !std::is_base_of_v<family_type type>,
		"compile-time-error : weak_ref::dynamic_cast_to family_type is not compatible with type !"
	);

	return weak_ref<family_type>(D_CAST(this->memory, family_type*));
}


#ifdef DEBUG

WEAK_REF_TEMPLATE 
DEBUG_ONLY INLINE u32 weak_ref<type>::get_strong_count() NOEXP {
	return (this->ctr) ? this->ctr->__strong__ : 0u;
}

WEAK_REF_TEMPLATE 
DEBUG_ONLY INLINE u32 weak_ref<type>::get_strong_count() const NOEXP {
	return (this->ctr) ? this->ctr->__strong__ : 0u;
}

WEAK_REF_TEMPLATE 
DEBUG_ONLY INLINE u32 weak_ref<type>::get_weak_count() NOEXP {
	return (this->ctr) ? this->ctr->__weak__ : 0u;
}

WEAK_REF_TEMPLATE 
DEBUG_ONLY INLINE u32 weak_ref<type>::get_weak_count() const NOEXP {
	return (this->ctr) ? this->ctr->__weak__ : 0u;
}

#endif 

namespace core {

	namespace make {

		template<typename type> weak_ref<type> 
		weak_ref_from_shared(shared_ref<type> const& reference) NOEXP {

			return weak_ref<type>(reference);
		}

	} // namespace make end

} // namespace core end


#endif