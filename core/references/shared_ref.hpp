#pragma once 

#ifndef CORE_REF_HPP
#define CORE_REF_HPP

#include "core/macros.hpp"
#include "core/logger/logger.hpp"
#include "core/status/status.hpp"

#include "ref_counter.hpp"

CORE_GET_LOGGER_VAR(_shared_ref_ctr_logger_, REFS_LOGGER);
#define _LOGGER_ _shared_ref_ctr_logger_

/*
	"intrusive" shared_ref counting class !
	- used to manage the life-time of shared memory .
	- memory life-time get managed automatically depened on how many ref's alive pointing at the memory .
	- important-note: any type should obey the rules of "refcounted_type concept" .
*/
template<refcounted_type type> class shared_ref {

private:
	type* memory = nullptr;

public:
	// constructor's
	shared_ref() = default;
	shared_ref(type* pointer) NOEXP;
	shared_ref(shared_ref<type> const& other) NOEXP; // copy
	shared_ref(shared_ref<type>&& other) NOEXP; // move

	// note: use this for upcasting "base <-- derived"
	// todo[important]: check if base_type is from refcounter
	template<typename base_type> shared_ref(base_type* pointer) NOEXP;
	template<typename base_type> shared_ref(shared_ref<base_type> const& other);

	// operator's 
	shared_ref<type>& operator= (shared_ref<type> const& other) NOEXP;
	shared_ref<type>& operator= (shared_ref<type>&& other) NOEXP;

	type* operator->() NOEXP;
	type* operator->() NOEXP const;

	type& operator *();
	type& operator *() const;

	bool  operator bool() NOEXP;

	// destructor
	~shared_ref();

	// note: use this to do downcasting "base --> derived"
	// note: don't use dynamic_cast a lot to avoid preformance costs
	// todo: don't forget to increment __strong__ counter , you're giving a new ref<family_type> !
	template<typename family_type> shared_ref<family_type> dynamic_cast();

// debug-only functions
#ifdef DEBUG
	INLINE u32 get_strong_count() NOEXP;
	INLINE u32 get_strong_count() const NOEXP;

	INLINE u32 get_weak_count() NOEXP;
	INLINE u32 get_weak_count() const NOEXP;
#endif

}; 
// class shared_ref end


// few function for shared refernces
namespace core {

	namespace make {
	
		template<refcounted_type type, typename... constructor_parameters> 
		shared_ref<type> shared_refernce(constructor_parameters&&... parameters) {

			// todo: implement this !

		}

	}

} // namespace core end


/*
	class shared_ref implementation 
*/

template<refcounted_type type>  
shared_ref<type>::shared_ref(type* pointer) NOEXP : memory(pointer) {
	this->memory->__strong__ += 1;
}

template<refcounted_type type>  
shared_ref<type>::shared_ref(shared_ref<type> const& other) NOEXP { // copy
	this->memory = other.memory;
	this->memory->__strong__ += 1;
}

template<refcounted_type type>  
shared_ref<type>::shared_ref(shared_ref<type>&& other) NOEXP { // move
	this->memory = other.memory;
	other.memory = nullptr;
}

template<refcounted_type type>
shared_ref<type>::~shared_ref() {
	// todo: maybe implement thread-safe destruction 
	//       to avoid object multi-destruction

	if (this->memory->__strong__) this->memory->__strong__ -= 1;
#ifdef DEBUG
	else {
		// bug -> probably cause by destructor begin called multiple times somewhere !
	}
#endif

	// if strong and weak is zero 
	if ((!this->memory->__strong__) && (!this->memory->__weak__) ) {
		// destruct object in memory
		// deallocating memory is object responsiblity
		this->memory->~type();
	}

}


/*
	debug-only functions
*/
#ifdef DEBUG
template<refcounted_type type> INLINE u32 shared_ref<type>::get_strong_count() NOEXP {
	return this->__strong__;
}
template<refcounted_type type> INLINE u32 shared_ref<type>::get_strong_count() const NOEXP {
	return this->__strong__;
}

template<refcounted_type type> INLINE u32 shared_ref<type>::get_weak_count() NOEXP {
	return this->__weak__;
}
template<refcounted_type type> INLINE u32 shared_ref<type>::get_weak_count() const NOEXP {
	return this->__weak__;
}
#endif


/*
	shared_ref public functions
*/ 



#endif
