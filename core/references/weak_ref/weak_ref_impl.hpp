#pragma once
 
#ifndef CORE_WEAK_REF_IMPL_HPP
#define CORE_WEAK_REF_IMPL_HPP

#include "weak_ref.hpp"
#include "core/status/status.hpp"
#include "core/logger/logger.hpp"

#ifndef _LOGGER_
	#define _LOGGER_ nullptr
#endif

/*
			constructor's
*/

template<refcounted_type type>	
weak_ref<type>::weak_ref(shared_ref<type> const& other_ref) NOEXP {

	if (other_ref.memory != nullptr) {
		this->memory = other_ref.memory;
		ADD_WEAK_REF_ATOMIC(this);
	}
#ifdef DEBUG
	else {
		CORE_WARN_D("nullptr memory passed to weak_ref<{}> during construction time !" , type_name);
		CORE_STACK_TRACE();
	}
#endif
}

template<refcounted_type type>
weak_ref<type>::weak_ref(type* pointer) NOEXP {

	if (pointer != nullptr) {
		this->memory = pointer;
		ADD_WEAK_REF_ATOMIC(this);
	}
#ifdef DEBUG
	else {
		CORE_WARN_D("nullptr memory passed to weak_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE();
	}
#endif
}

template<refcounted_type type>
weak_ref<type>::weak_ref(weak_ref<type> const& other_ref) NOEXP {
	
	if (other_ref.memory != nullptr) {
		this->memory = other_ref.memory;
		ADD_WEAK_REF_ATOMIC(this);
	}
#ifdef DEBUG
	else {
		CORE_WARN_D("nullptr memory passed to weak_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE();
	}
#endif
}

template<refcounted_type type>
weak_ref<type>::weak_ref(weak_ref<type>&& ref) NOEXP {

	if (ref.memory != nullptr) {
		this->memory = ref.memory;
		ref.memory   = nullptr;
	}
#ifdef DEBUG
	else {
		CORE_WARN_D("nullptr memory passed to weak_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE();
	}
#endif
}

/*
	destructor
*/
template<refcounted_type type>
weak_ref<type>::~weak_ref( ) NOEXP {

	if (this->memory) {

		// if no ref is alive
		if ((this->memory->__strong__ == 0) && (SUB_WEAK_REF_ATOMIC(this) == 0) ) {
			this->memory->~type();
		}

	}
#ifdef DEBUG
	else {
		CORE_WARN_D("weak_ptr<{}> memory pointer is nullptr during destruction time !" , type_name);
		CORE_STACK_TRACE();
	}
#endif 

}

/*
	upcasting constructor's
*/
template<refcounted_type type>
template<std::derived_from<type> derived_type>
weak_ref<type>::weak_ref(derived_type* pointer) NOEXP {

	if (pointer != nullptr) {
		this->memory = pointer;
		ADD_WEAK_REF_ATOMIC(this);
	}
#ifdef DEBUG
	else {
		CORE_WARN_D("nullptr memory passed to weak_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE();
	}
#endif

}

template<refcounted_type type>
template<std::derived_from<type> derived_type>
weak_ref<type>::weak_ref(weak_ref<derived_type> const& derived) NOEXP {

	if (derived.memory != nullptr) {
		this->memory = derived.memory;
		ADD_WEAK_REF_ATOMIC(this);
	}
#ifdef DEBUG
	else {
		CORE_WARN_D("nullptr memory passed to weak_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE();
	}
#endif
}

template<refcounted_type type>
template<std::derived_from<type> derived_type>
weak_ref<type>::weak_ref(weak_ref<derived_type>&& derived) NOEXP {

	if (derived.memory != nullptr) {
		this->memory = derived.memory;
		derived.memory = nullptr;
	}
#ifdef DEBUG
	else {
		CORE_WARN_D("nullptr memory passed to weak_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE();
	}
#endif
}


/*
			operators
*/
	
template<refcounted_type type>  // copy operator
weak_ref<type>& weak_ref<type>::operator=(weak_ref<type> const& other) NOEXP { 

	if (other.memory != nullptr) {
		this->memory = other.memory;
		ADD_WEAK_REF_ATOMIC(this);
	}
#ifdef DEBUG
	else {
		CORE_WARN_D("nullptr memory passed to weak_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE();
	}
#endif
}

template<refcounted_type type>  // copy operator
weak_ref<type>& weak_ref<type>::operator=(shared_ref<type> const& other) NOEXP {

	if (other.memory != nullptr) {
		this->memory = other.memory;
		ADD_WEAK_REF_ATOMIC(this);
	}
#ifdef DEBUG
	else {
		CORE_WARN_D("nullptr memory passed to weak_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE();
	}
#endif
}

template<refcounted_type type> // assign operator
weak_ref<type>& weak_ref<type>::operator=(weak_ref<type>&& ref) NOEXP {

	if (other.memory != nullptr) {
		this->memory = other.memory;
		ref.memory   = nullptr;
	}
#ifdef DEBUG
	else {
		CORE_WARN_D("nullptr memory passed to weak_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE();
	}
#endif
}

template<refcounted_type type> 
type* weak_ref<type>::operator->() NOEXP {
	return this->memory;
}

template<refcounted_type type> 
const type* weak_ref<type>::operator->() const NOEXP {
	return (const type *)this->memory;
}

template<refcounted_type type> 
type& weak_ref<type>::operator*() NOEXP {

#ifdef DEBUG
	CORE_FATAL_IF(!this->memory, CORE_LOG_CONFIG_ALL , "attempt to decrefernce nullptr memory in weak_ref<{}> !" , type_name);
#endif

	return *this->memory;
}

template<refcounted_type type> 
const type& weak_ref<type>::operator*() const NOEXP {
#ifdef DEBUG
	CORE_FATAL_IF(!this->memory, CORE_LOG_CONFIG_ALL, "attempt to decrefernce nullptr memory in weak_ref<{}> !", type_name);
#endif
	
	return *this->memory;
}

template<refcounted_type type> 
weak_ref<type>::operator bool() const NOEXP {
	return (this->memory != nullptr);
}

/*
	function's
*/

// note: use this to do downcasting "base --> derived"
// note: don't use dynamic_cast_to a lot to avoid preformance costs
template<refcounted_type type>
template<typename family_type>
weak_ref<family_type> weak_ref<type>::dynamic_cast_to() {

	COMPILE_TIME_ASSERT(
		!std::is_base_of_v<type, family_type> && !std::is_base_of_v<family_type, type>,
		"shared_ref<type> is not related to {} type !", TYPE_NAME(family_type)
	);

	return weak_ref<family_type>(D_CAST(this->memory, family_type*));
}



#ifdef DEBUG
template<refcounted_type type> 
DEBUG_ONLY INLINE u32 weak_ref<type>::get_strong_count() NOEXP {
	return (this->memory) ? this->memory->__strong__ : 0u;
}

template<refcounted_type type> 
DEBUG_ONLY INLINE u32 weak_ref<type>::get_strong_count() const NOEXP {
	return (this->memory) ? this->memory->__strong__ : 0u;
}

template<refcounted_type type> 
DEBUG_ONLY INLINE u32 weak_ref<type>::get_weak_count() NOEXP {
	return (this->memory) ? this->memory->__weak__ : 0u;
}

template<refcounted_type type> 
DEBUG_ONLY INLINE u32 weak_ref<type>::get_weak_count() const NOEXP {
	return (this->memory) ? this->memory->__weak__ : 0u;
}

#endif 


#endif