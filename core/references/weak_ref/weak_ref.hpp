#pragma once 

#ifndef CORE_WEAK_REF_HPP
#define CORE_WEAK_REF_HPP

#include "core/references/ref_counter.hpp"
#include "core/strings/string.hpp"

template<refcounted_type type> class shared_ref;

// note: weak_ref/pointer have no ownership over memory
// note: this help solve circular dependency problem when --> A refer to B and B to A !! causing memory leak !
template<refcounted_type type> class weak_ref {

private:
	static inline const STRING type_name = typeid(type).name();

	type* memory = nullptr;

	weak_ref() = delete;
	template<refcounted_type __TYPE__> friend class shared_ref;

public:
	// constructor's
	weak_ref(type* pointer) NOEXP;
	weak_ref(weak_ref<type>&& ref) NOEXP;
	weak_ref(weak_ref<type>   const& other_ref) NOEXP;
	weak_ref(shared_ref<type> const& other_ref) NOEXP;

	// note: use this for upcasting "base <-- derived"
	template<std::derived_from<type> derived_type> weak_ref(derived_type* pointer) NOEXP;
	template<std::derived_from<type> derived_type> weak_ref(weak_ref<derived_type> const& derived) NOEXP;
	template<std::derived_from<type> derived_type> weak_ref(weak_ref<derived_type> && derived) NOEXP;
	
	// destructor
	~weak_ref() NOEXP;

	// operator's
	weak_ref<type>& operator=(weak_ref<type>   const& other) NOEXP; // copy
	weak_ref<type>& operator=(shared_ref<type> const& other) NOEXP; // copy
	weak_ref<type>& operator=(weak_ref<type>&& ref) NOEXP; // assign

	      type* operator->() NOEXP;
	const type* operator->() const NOEXP;
	
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


#include "weak_ref_impl.hpp"

#endif