#pragma once 

#ifndef CORE_UNIQUE_REF_HPP
#define CORE_UNIQUE_REF_HPP

#include "core/macros.hpp"

/*
	unique ref for fully ownership by one entity
*/
template<typename type> class unique_ref {
private:
	static const char* type_name = typeid(type).name;

	type* memory = nullptr;

	unique_ref() = delete;
public:
	// constructor's
	unique_ref(type* pointer) NOEXP;
	unique_ref(unique_ref<type>&  other) NOEXP;
	unique_ref(unique_ref<type>&& other) NOEXP;

	// destructor
	~unique_ref( ) NOEXP;

	// operator's
	unique_ref<type>& operator = (type* pointer) NOEXP;
	unique_ref<type>& operator = (unique_ref<type>&  other) NOEXP;
	unique_ref<type>& operator = (unique_ref<type>&& other) NOEXP;

	type* operator->() NOEXP;
	type& operator*()  NOEXP;
	const type& operator*()  const NOEXP;

	operator bool() const NOEXP;

	// public function's

}; 
// class unique_ref end


#include "unique_ref_impl.hpp"

#endif
