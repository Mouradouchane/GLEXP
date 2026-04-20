#pragma once 

#ifndef CORE_REF_HPP
#define CORE_REF_HPP

#include "ref_counter.hpp"


// few rules/demands here for template type for class ref
template<typename type> concept refcounted_type = requires(type t) {
	typename type::__is_refcounter_type__;

	requires std::has_virtual_destructor_v<type>;
	requires std::is_nothrow_destructible_v<type>;

} && std::destructible<type>;


// inject this macro at the end of your class !
// to make it acceptable by "ref counter classes"
#define INJECT_REF_COUNTER(THIS_TYPE) \
		public: \
			using __is_refcounter_type__ = void; \
		protected : \
			template<refcounted_type THIS_TYPE> friend class ref; \
			std::atomic<u32> __strong__ = 0; \
			std::atomic<u32>  __weak__  = 0;

/*
	ref is a "shared/intrusive ref counting" class , used to manage the life-time of shared-object/memory .
	object life-time get managed automatically depened on how many ref's alive pointing at the object .
	important-note: type should obey the rules/demands above in "refcounted_type concept" .
*/
template<refcounted_type type> class ref {

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
	// todo: implement thread-safe destruction to avoid object multi-destruction
	~ref();

	// ref functions
	inline type* get() NOEXP;
	inline type* get() NOEXP const;

	// note: use this to do downcasting "base --> derived"
	// note: don't use dynamic_cast a lot to avoid preformance costs
	// todo: don't forget to increment __strong__ counter , you're giving a new ref<family_type> !
	template<typename family_type> ref<family_type> dynamic_cast();

	// debug-only functions
#ifdef DEBUG
	INLINE u32 get_strong_count() NOEXP;
	INLINE u32 get_strong_count() const NOEXP;

	INLINE u32 get_weak_count() NOEXP;
	INLINE u32 get_weak_count() const NOEXP;
#endif

	// static function's
	template<typename... constructor_parameters> 
	static ref<type> make_ref(constructor_parameters&& parameters);
}; 
// class ref definition end


/*
	class ref implementation 
*/

#endif
