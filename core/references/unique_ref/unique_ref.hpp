#pragma once 

#ifndef CORE_UNIQUE_REF_HPP
#define CORE_UNIQUE_REF_HPP

#include "core/macros.hpp"
#include "core/strings/string.hpp"

/*
	unique ref for fully ownership by one entity
*/
template<typename type> class unique_ref {
private:
	static inline const STRING type_name = typeid(type).name();

	type* memory = nullptr;

	unique_ref() = delete;
	unique_ref(unique_ref<type>&  other) = delete;
	unique_ref<type>& operator = (unique_ref<type>& other) = delete;

public:
	// constructor's
	unique_ref(type* pointer) NOEXP;
	unique_ref(unique_ref<type>&& other) NOEXP;

	// destructor
	~unique_ref( ) NOEXP;

	// operator's
	unique_ref<type>& operator = (type* pointer) NOEXP;
	unique_ref<type>& operator = (unique_ref<type>&& other) NOEXP;

	type* operator->() NOEXP;
	type& operator*()  NOEXP;
	const type& operator*()  const NOEXP;

	operator bool() const NOEXP;

	// public function's
	type* pass_ownership() NOEXP;

	static bool move_ownership(unique_ref<type>& ref , unique_ref<type>& new_owner);
}; 
// class unique_ref end


#include "unique_ref_impl.hpp"

#endif
