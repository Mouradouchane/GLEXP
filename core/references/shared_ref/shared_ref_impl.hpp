#pragma once 

#ifndef CORE_SHARED_REF_IMPL
#define CORE_SHARED_REF_IMPL

// just to stop syntax-errors :)
#include "shared_ref.hpp"

/*

	shared_ref constructor's

*/

template<refcounted_type type>
shared_ref<type>::shared_ref(type* pointer) NOEXP {
	this->memory = pointer;
	if (this->memory) ADD_SHARED_REF_ATOMIC(this);
	else {
		CORE_WARN("nullptr memory passed to shared_ref<{}> during construction !" , typeid(type).name);
	}
}

template<refcounted_type type>
shared_ref<type>::shared_ref(shared_ref<type> const& other) NOEXP { // copy

	if (other.memory) {
		this->memory = other.memory;
		ADD_SHARED_REF_ATOMIC(this);
	}
	else {
		CORE_WARN("nullptr memory passed to shared_ref<{}> during construction !", typeid(type).name);
	}

}

template<refcounted_type type>
shared_ref<type>::shared_ref(shared_ref<type>&& other) NOEXP { // move

	if (other.memory) {
		this->memory = other.memory;
		other.memory = nullptr;
	}
	else {
		CORE_WARN("nullptr memory passed to shared_ref<{}> during construction !", typeid(type).name);
	}

}

/*

	shared_ref upcasting constructor's

*/

// note: use this for upcasting "base <-- derived"
template<refcounted_type  type>
template<std::derived_from<type> derived_type>
shared_ref<type>::shared_ref(derived_type* pointer) NOEXP {

	this->memory = pointer;
	if (this->memory) ADD_SHARED_REF_ATOMIC(this);
}

// note: use this for upcasting "base <-- derived"
template<refcounted_type  type>
template<std::derived_from<type> derived_type>
shared_ref<type>::shared_ref(shared_ref<derived_type> const& other) NOEXP { // copy

	this->memory = other.memory;
	if (this->memory) ADD_SHARED_REF_ATOMIC(this);
}

/*

	class shared_ref destructor

*/
template<refcounted_type type>
shared_ref<type>::~shared_ref() {
	// todo: implement thread-safe destruction , to avoid object multi-destruction
	
	if (this->memory) {

		// if no refernce is left
		if (SUB_SHARED_REF_ATOMIC(this) == 1 && (this->memory->__weak__ == 0)) {

			this->memory->~type();
			this->memory = nullptr;
		}

	}
#ifdef DEBUG
	else {
		CORE_WARN("shared_ptr<{}> : memory-block pointer is nullptr during destruction !" , TYPE_NAME(type));
	}
#endif
} // shared_ref destructor end			


/*

	shared_ref public functions

*/


// note: use this to do downcasting "base --> derived"
// note: don't use dynamic_cast_to a lot to avoid preformance costs
template<refcounted_type type> 
template<typename family_type> 
shared_ref<family_type> shared_ref<type>::dynamic_cast_to() {

	COMPILE_TIME_ASSERT(
		!std::is_base_of_v<type, family_type> && !std::is_base_of_v<family_type, type> ,
		"shared_ref<type> is not related to {} type !" , TYPE_NAME(family_type)
	);
	
	return shared_ref<family_type>( D_CAST(this->memory, family_type*) );
}


// inline heleper function used to deal with already exist reference
template<refcounted_type type>
INLINE void shared_ref<type>::deal_with_current_refernce() NOEXP {

	if (this->memory) {

		// destruct if no ref is left
		if (SUB_SHARED_REF_ATOMIC(this) == 1 && (this->memory->__weak__ == 0)) {

			this->memory->~type();
			this->memory = nullptr;
		}
	}

}

/*

	class shared_ref operator's

*/

// assing operator
template<refcounted_type type> 
shared_ref<type>& shared_ref<type>::operator= (shared_ref<type> const& other) NOEXP { 
	
	if (this->memory == other.memory) {
		CORE_WARN(core::status::get_warning(core::warning::self_assignment));
	}
	else {
		shared_ref<type>::deal_with_current_refernce();

		this->memory = other.memory;
		if (this->memory) SUB_SHARED_REF_ATOMIC(this);
	}
	
	return *this;
}

template<refcounted_type type>
shared_ref<type>& shared_ref<type>::operator= (shared_ref<type>&& other) NOEXP { // move

	if (this->memory == other.memory) {
		CORE_WARN("{}", core::status::get_warning(core::warning::self_assignment));
	}
	else {
		shared_ref<type>::deal_with_current_refernce();

		this->memory = other.memory;
		other.memory = nullptr;
	}

	return *this;
}

template<refcounted_type type>
type* shared_ref<type>::operator-> () NOEXP {
	return this->memory;
}

template<refcounted_type type>
const type* shared_ref<type>::operator-> () const NOEXP {
	return C_CAST(this->memory, const type*);
}

template<refcounted_type type>
type& shared_ref<type>::operator* () {
	return *this->memory;
}

template<refcounted_type type>
const type& shared_ref<type>::operator* () const {
	return C_CAST(*this->memory, const type&);
}

template<refcounted_type type>
shared_ref<type>::operator bool() const NOEXP {
	return this->memory != nullptr;
}


// debug-only functions
#ifdef DEBUG
template<refcounted_type type> 
DEBUG_ONLY INLINE u32 shared_ref<type>::get_strong_count() NOEXP {
	return this->memory->__strong__;
}

template<refcounted_type type>
DEBUG_ONLY INLINE u32 shared_ref<type>::get_strong_count() const NOEXP {
	return this->memory->__strong__;
}

template<refcounted_type type> 
DEBUG_ONLY INLINE u32 shared_ref<type>::get_weak_count() NOEXP {
	return this->memory->__weak__;
}

template<refcounted_type type> 
DEBUG_ONLY INLINE u32 shared_ref<type>::get_weak_count() const NOEXP {
	return this->memory->__weak__;
}
#endif

template<
	refcounted_type type,
	typename...     constructor_parameters
>
shared_ref<type> core::make::shared_refernce(core::memory_allocator& allocator, constructor_parameters&&... parameters) {

	// allocate memory
	type* memory = S_CAST(allocator.allocate(), type*);

	// construct memory
	new (memory) type(std::forward<constructor_parameters>(parameters)...);

	return shared_ref<type>(memory);
}

#endif