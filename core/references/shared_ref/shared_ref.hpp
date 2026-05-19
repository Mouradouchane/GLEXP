#pragma once 

#ifndef CORE_REF_HPP
#define CORE_REF_HPP

#include "core/macros.hpp"
#include "core/logger/logger.hpp"

#include "core/references/ref_counter.hpp"

/*
	* shared_ref is "intrusive" and "strong" refernce-counter.
	- used to manage the life-time and access of shared memory.
	- memory life-time get managed automatically depened on how many ref's still alive pointing at the memory.
*/
template<typename type> class shared_ref {

private:
	requires (std::is_default_constructible<type>::value);

	template<typename type> friend class weak_ref;
	template<typename type> friend class shared_ref;

	static inline const STRING type_name = typeid(type).name();

	type*          memory;
	counter_block* ctr;
	
	// private constructors
	shared_ref( ) = delete;
	shared_ref(counter_block* ctr_ptr, type* memory_ptr);

public:
	static const type __dummy__ = type(); // used to check if what you dereferenced is valid

	// constructor's
	template<typename... parameters> 
	shared_ref(core::memory_allocator const& allocator , parameters&&... constructor_parameters) NOEXP;

	shared_ref(shared_ref<type> const& reference) NOEXP; // copy
	shared_ref(shared_ref<type> &&     reference) NOEXP; // move

	// note: use this for upcasting "base <-- derived"
	template<std::derived_from<type> derived_type> shared_ref(shared_ref<derived_type> const& reference) NOEXP;
	template<std::derived_from<type> derived_type> shared_ref(shared_ref<derived_type> &&     reference) NOEXP;

	// destructor
	~shared_ref();

	// operator's 
	shared_ref<type>& operator= (shared_ref<type> const& reference) NOEXP; // copy
	shared_ref<type>& operator= (shared_ref<type> &&     reference) NOEXP; // assign

	// upcasting assign operators
	template<std::derived_from<type> derived_type>
	shared_ref<type>& operator=(shared_ref<derived_type> const& reference) NOEXP;

	template<std::derived_from<type> derived_type>
	shared_ref<type>& operator=(shared_ref<derived_type>&& reference) NOEXP;

	type*       operator->() NOEXP;
	const type* operator->() const NOEXP;

	type&       operator *() NOEXP;
	const type& operator *() const NOEXP;

	operator bool() const NOEXP;

	// note: use this to do downcasting "base --> derived"
	// note: don't use dynamic_cast_to a lot to avoid preformance costs
	template<typename family_type> 
	shared_ref<family_type> dynamic_cast_to();

// debug-only functions
#ifdef DEBUG
	DEBUG_ONLY INLINE u32 get_strong_count() NOEXP;
	DEBUG_ONLY INLINE u32 get_strong_count() const NOEXP;

	DEBUG_ONLY INLINE u32 get_weak_count() NOEXP;
	DEBUG_ONLY INLINE u32 get_weak_count() const NOEXP;
#endif


private: // private helper function
	INLINE void deal_with_current_reference() NOEXP;

}; 
// class shared_ref end


/*

	few function for shared refernces

*/
namespace core {

	namespace make {

		// this for type and 
		template<typename type, typename... parameters> 
		shared_ref<type> shared_reference(core::memory_allocator const& allocator, parameters&&... parameters);

	} // namespace make end

} // namespace core end


// shared_ref template implementation is here
#include "shared_ref_impl.hpp"

#endif
