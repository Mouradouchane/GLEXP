#pragma once 

#ifndef CORE_REF_HPP
#define CORE_REF_HPP

#include "core/macros.hpp"
#include "core/logger/logger.hpp"
#include "core/status/status.hpp"

#include "core/references/ref_counter.hpp"

CORE_GET_LOGGER_VAR(_shared_ref_ctr_logger_, REFS_LOGGER);
#define _LOGGER_ _shared_ref_ctr_logger_


/*
	* shared_ref is "intrusive" and "strong" refernce-counter.
	- used to manage the life-time of shared memory.
	- memory life-time get managed automatically depened on how many ref's still alive pointing at the memory.
*/
template<refcounted_type type> class shared_ref {

private:
	type* memory = nullptr;

	shared_ref( ) = delete;
	template<refcounted_type __TYPE__> friend class weak_ref;

public:
	// constructor's
	shared_ref(type* pointer) NOEXP;
	shared_ref(shared_ref<type> const& other) NOEXP; // copy
	shared_ref(shared_ref<type> &&     other) NOEXP; // move

	// note: use this for upcasting "base <-- derived"
	template<std::derived_from<type> derived_type> shared_ref(derived_type* pointer) NOEXP;
	template<std::derived_from<type> derived_type> shared_ref(shared_ref<derived_type> const& other) NOEXP;

	// destructor
	~shared_ref();

	// operator's 
	shared_ref<type>& operator= (shared_ref<type> const& other) NOEXP; // copy
	shared_ref<type>& operator= (shared_ref<type> &&     other) NOEXP; // assign

	      type* operator->() NOEXP;
	const type* operator->() const NOEXP;

	      type& operator *();
	const type& operator *() const;

	operator bool() const NOEXP;

	// note: use this to do downcasting "base --> derived"
	// note: don't use dynamic_cast_to a lot to avoid preformance costs
	template<typename family_type> 
	shared_ref<family_type> dynamic_cast_to();

// debug-only functions
#ifdef DEBUG
	DEBUG_ONLY  INLINE u32 get_strong_count() NOEXP;
	DEBUG_ONLY  INLINE u32 get_strong_count() const NOEXP;

	DEBUG_ONLY  INLINE u32 get_weak_count() NOEXP;
	DEBUG_ONLY  INLINE u32 get_weak_count() const NOEXP;
#endif


private: // private helper function
	INLINE void deal_with_current_refernce() NOEXP;

}; 
// class shared_ref end


/*

	few function for shared refernces

*/
namespace core {

	namespace make {

		// this for type and 
		template<refcounted_type type, typename... constructor_parameters> 
		shared_ref<type> shared_refernce(core::memory_allocator& allocator, constructor_parameters&&... parameters);

	} // namespace make end

} // namespace core end


// tempaltes implementation is here 
#include "shared_ref_impl.hpp"

#endif
