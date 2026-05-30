#pragma once 

#ifndef CORE_UNIQUE_REF_HPP
#define CORE_UNIQUE_REF_HPP

#include "core/macros.hpp"
#include "core/assert.hpp"
#include "core/strings/string.hpp"

#include "core/references/ref_counter.hpp"


/*
 
	unique_ref for fully ownership by one entity

*/

#define UNIQUE_REF_TEMPLATE template<typename type>

UNIQUE_REF_TEMPLATE
class unique_ref {

private:
	COMPILE_TIME_ASSERT(
		(!std::is_default_constructible<type>::value) , 
		"no default constructor found in type used with unique_ref"
	);

	static inline const STRING type_name = typeid(type).name();

	core::memory_allocator* allocator = nullptr;
 	type* memory = nullptr;

	// not allowed constructors/operators
	unique_ref() = delete;
	unique_ref(unique_ref<type> const& other) = delete;
	unique_ref<type>& operator = (unique_ref<type>& other) = delete;
	unique_ref<type>& operator = (unique_ref<type> const& other) = delete;

	unique_ref(core::memory_allocator* allocator , type* memory_ptr) NOEXP;

public:
	static const type __dummy__();

	// constructor's
	template<typename... parameters> 
	unique_ref(core::memory_allocator const& allocator , parameters&&... constructor_parameters) NOEXP;
	unique_ref(unique_ref<type> && unique_reference) NOEXP;
	unique_ref(unique_ref<type> &  unique_reference) NOEXP;

	// destructor
	~unique_ref( ) NOEXP;

	// operator's
	unique_ref<type>& operator = (unique_ref<type>&& unique_reference) NOEXP; // move

	type* operator->() NOEXP;
	type& operator*()  NOEXP;
	const type& operator*() const NOEXP;

	operator bool() const NOEXP;

	// public function's
	duo_pointers<core::memory_allocator, type> pass_ownership() NOEXP;

	static bool move_ownership(unique_ref<type>& reference , unique_ref<type>& new_owner);

private :
	INLINE void deal_with_current_reference() NOEXP;

}; 
// class unique_ref end


#include "unique_ref_impl.hpp"

#endif
