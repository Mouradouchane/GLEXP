#pragma once 

#ifndef CORE_REF_HPP
#define CORE_REF_HPP

#include "ref_counter.hpp"

/*
	class ref --> intrusive and shared
*/
template<typename type> class ref { 
	// note : type should inherite from refcounter before using it with ref
	COMPILE_TIME_ASSERT(
		!std::is_base_of<refcounter, type>::value,
		"class ref : " + typeid(type).name() + " type should inherite class refcounter !"
	);

protected:
	type* object = nullptr;

public:
	// constructor's
	ref() = default;
	ref(type* pointer) NOEXP;
	ref(ref<type> const& other) NOEXP; // copy
	ref(ref<type>&& other) NOEXP; // move

	// note: use this for upcasting "base <-- derived"
	// todo[important]: check if base_type is from refcounter
	template<typename base_type> ref(base_type* pointer) NOEXP;
	template<typename base_type> ref(ref<base_type> const& other);

	// operator's 
	ref<type>& operator= (ref<type> const& other) NOEXP;
	ref<type>& operator= (ref<type>&& other) NOEXP;

	type* operator->() NOEXP;
	type* operator->() NOEXP const;

	type& operator *();
	type& operator *() const;

	bool  operator bool() NOEXP;

	// destructor
	~ref();

	// ref functions
	inline type* get() NOEXP;
	inline type* get() NOEXP const;

	// note: use this to do downcasting "base --> derived"
	// note: don't use dynamic_cast a lot to avoid preformance costs
	template<typename family_type> ref<family_type> dynamic_cast();

}; 
// class ref definition end


/*
	class ref implementation 
*/

//
#endif