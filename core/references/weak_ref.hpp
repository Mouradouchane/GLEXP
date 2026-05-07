#pragma once 

#ifndef CORE_WEAK_REF_HPP
#define CORE_WEAK_REF_HPP

#include "ref_counter.hpp"

template<refcounted_type type> class shared_ref;

// note: weak_ref/pointer have no ownership over memory
// note: this help solve circular dependency problem when --> A refer to B and B to A !! causing memory leak !
template<refcounted_type type> class weak_ref {
private:
	type* memory = nullptr;

	weak_ref() = delete;

public:
	// constructor's
	 weak_ref(shared_ref<type> other_ref);
	 
	 weak_ref(type* pointer);
	 weak_ref(weak_ref<type> const& other_ref);
	 weak_ref(weak_ref<type>&& ref);

	// destructor
	~weak_ref();

	// public functions


}; // class weak_ref end

#endif