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
	requires (std::is_default_constructible<type>::value);

private:
	static inline const STRING type_name = typeid(type).name();

	// note: one block of memory used as tow blocks
	counter_block* ctr;
	type*          memory;

	weak_ref() = delete;
	template<typename type> friend class shared_ref;

	// private constructors
	template<std::derived_from<type> derived_type> 
	weak_ref(derived_type* derived_ref_memory) NOEXP;
	weak_ref(type* other_ref_memory) NOEXP;

public:	
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
	
	// note: use this dummy object to check if whats on memory is still alive or not
	static const type __dummy__ = type();

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

}; // class weak_ref end


namespace core {

	namespace make {

		template<typename type> weak_ref<type> weak_ref_from_shared(shared_ref<type> const& reference) NOEXP;

	}

}

#include "weak_ref_impl.hpp"

#endif