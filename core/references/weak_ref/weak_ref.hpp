#pragma once  

#ifndef CORE_WEAK_REF_HPP
#define CORE_WEAK_REF_HPP

#include "core/references/ref_counter.hpp"
#include "core/strings/string.hpp"

template<typename type> class shared_ref;

/*
	note: weak_ref have no ownership , used as handle to access memory "read/write"
	note: this help solve circular dependency problem when --> A refer to B and B to A !! causing memory leak !
*/
template<typename type> class weak_ref {
	// requires std::is_default_constructible<type>::value;

private:
	static inline const STRING type_name = typeid(type).name();

	template<typename type> friend class shared_ref;
	template<typename type> friend class weak_ref;

	type*          memory;
	counter_block* ctr;

	// private constructors
	weak_ref() = delete;
	weak_ref(counter_block* ctr_ptr, type* ref_memory) NOEXP;

	template<std::derived_from<type> derived_type> 
	weak_ref(counter_block* ctr_ptr, derived_type* derived_memory) NOEXP;

public:	
	static const type __dummy__(); // used to check if what you dereferenced is valid

	// constructor's
	weak_ref(shared_ref<type> const& reference) NOEXP;
	weak_ref(weak_ref<type>   const& reference) NOEXP;
	weak_ref(weak_ref<type>   &&     reference) NOEXP;

	// note: use this for upcasting "base <-- derived"
	template<std::derived_from<type> derived_type> weak_ref(shared_ref<derived_type> const& derived_reference) NOEXP;
	template<std::derived_from<type> derived_type> weak_ref(weak_ref<derived_type>   const& derived_reference) NOEXP;
	template<std::derived_from<type> derived_type> weak_ref(weak_ref<derived_type>   &&     derived_reference) NOEXP;
	
	// destructor
	~weak_ref() NOEXP;

	// operator's
	weak_ref<type>& operator=(weak_ref<type>   const& other_reference) NOEXP; // copy
	weak_ref<type>& operator=(shared_ref<type> const& other_reference) NOEXP; // copy
	weak_ref<type>& operator=(weak_ref<type>   &&     other_reference) NOEXP; // move/assign
	
	type& operator*() NOEXP;
	const type& operator*() const NOEXP;

	operator bool() const NOEXP;

	// public function's

	// note: use this to do downcasting "base --> derived"
	// note: don't use dynamic_cast_to a lot to avoid preformance costs
	template<typename family_type> 
	weak_ref<family_type> dynamic_cast_to();

#ifdef DEBUG
	DEBUG_ONLY INLINE u32 get_strong_count() NOEXP;
	DEBUG_ONLY INLINE u32 get_strong_count() const NOEXP;

	DEBUG_ONLY INLINE u32 get_weak_count() NOEXP;
	DEBUG_ONLY INLINE u32 get_weak_count() const NOEXP;
#endif 

private: // helper functions
	INLINE void deal_with_current_reference() NOEXP;

}; // class weak_ref end

#include "weak_ref_impl.hpp"

#endif