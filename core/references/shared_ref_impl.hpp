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
	if (this->memory) this->memory->__strong__.fetch_add(1, std::memory_order_relaxed);
}

template<refcounted_type type>
shared_ref<type>::shared_ref(shared_ref<type> const& other) NOEXP { // copy

	if (other.memory) {
		this->memory = other.memory;
		this->memory->__strong__.fetch_add(1, std::memory_order_relaxed);
	}
	// todo: add log for invalid shared_ref
}

template<refcounted_type type>
shared_ref<type>::shared_ref(shared_ref<type>&& other) NOEXP { // move

	if (other.memory) {
		this->memory = other.memory;
		other.memory = nullptr;
	}
	// todo: add log for invalid shared_ref
}

/*

	shared_ref upcasting constructor's

*/

// note: use this for upcasting "base <-- derived"
template<refcounted_type   type>
template<std::derived_from<type> derived_type>
shared_ref<type>::shared_ref(derived_type* pointer) NOEXP {

	this->memory = pointer;
	if (this->memory) this->memory->__strong__.fetch_add(1, std::memory_order_relaxed);
}

// note: use this for upcasting "base <-- derived"
template<refcounted_type   type>
template<std::derived_from<type> derived_type>
shared_ref<type>::shared_ref(shared_ref<derived_type> const& other) { // copy

	this->memory = other.memory;
	if (this->memory) this->memory->__strong__.fetch_add(1, std::memory_order_relaxed);
}

/*

	class shared_ref destructor

*/
template<refcounted_type type>
shared_ref<type>::~shared_ref() {
	// todo: maybe implement thread-safe destruction , to avoid object multi-destruction

	if (this->memory) {

	#ifdef DEBUG // debug-only check for -> double destruction bug 
		if((!this->memory->__strong__) && (!this->memory->__weak__)) {
			CORE_ERROR_D("reference counters is 0 during destruction time !");
			CORE_WARN("BUG : this is probably cause by a bug in move/copy constructor's or operator's !");
		}
	#endif
		// todo: call destructor when strong hit 0 , deallocate when weak to hit 0
		// destruct if no ref is left
		if (this->memory->__strong__.fetch_sub(1, std::memory_order_acq_rel) == 1 && (!this->memory->__weak__)) {
			this->memory->~type();
			this->memory = nullptr;
		};

		return;
	}
	
	CORE_WARN_D("shared_ref memory is null !");

} // shared_ref destructor end


/*

	shared_ref public functions

*/


// note: use this to do downcasting "base --> derived"
// note: don't use dynamic_cast_to a lot to avoid preformance costs
template<refcounted_type type> 
template<typename family_type> 
shared_ref<family_type> shared_ref<type>::dynamic_cast_to() {

	// todo: compile-time check if family_type is valid 
	family_type* ptr = D_CAST(this->memory, family_type*) );

	return shared_ref<family_type>( ptr );
}


// inline heleper function used to deal with already exist reference
template<refcounted_type type>
INLINE void shared_ref<type>::deal_with_current_refernce() NOEXP {

	// check if some ref exist
	if (this->memory) {

		// destruct if no ref is left
		if (this->memory->__strong__.fetch_sub(1, std::memory_order_acq_rel) == 1 && (!this->memory->__weak__)) {

			// todo: handle memory deallocation if we move to global_allocator approch
			this->memory->~type();
			this->memory = nullptr;
		}

	}

}

/*

	class shared_ref operator's

*/

template<refcounted_type type>
shared_ref<type>& shared_ref<type>::operator= (shared_ref<type> const& other) NOEXP { // copy
	
	if (this->memory == other.memory) {
		CORE_WARN("{}", core::status::get_warning(core::warning::self_assignment));
	}
	else {
		shared_ref<type>::deal_with_current_refernce();

		this->memory = other.memory;
		if (this->memory) this->memory->__strong__.fetch_add(1, std::memory_order_relaxed);
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
template<refcounted_type type> INLINE u32 shared_ref<type>::get_strong_count() NOEXP {
	return this->memory->__strong__;
}
template<refcounted_type type> INLINE u32 shared_ref<type>::get_strong_count() const NOEXP {
	return this->memory->__strong__;
}

template<refcounted_type type> INLINE u32 shared_ref<type>::get_weak_count() NOEXP {
	return this->memory->__weak__;
}
template<refcounted_type type> INLINE u32 shared_ref<type>::get_weak_count() const NOEXP {
	return this->memory->__weak__;
}
#endif

template<
	refcounted_type type,
	typename...     constructor_parameters
>
shared_ref<type> core::make::shared_refernce(constructor_parameters&&... parameters) {

	// todo: this need implement !

}

#endif