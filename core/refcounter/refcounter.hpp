#pragma once 

#ifndef CORE_REFCOUNTER_HPP
#define CORE_REFCOUNTER_HPP

#include <type_traits>
#include <atomic>

#include "core/macros.hpp"
#include "core/assert.hpp"
#include "core/types.hpp"

/*

		here is a class for -> "intrusive refernce counter"
		used to keep track/managing objects life-time automatically

*/

class refcounter {
	protected :
		std::atomic<u32> count = 0;

		refcounter(const refcounter&) = delete;
		refcounter& operator=(const refcounter&) = delete;

	public :
		// constructor's
		refcounter() = default;
		refcounter(u32 _count_ = 0);
		
		// destructor
		virtual ~refcounter();

		// refcounter functions
		inline void add_ref() NOEXP;
		inline void release() NOEXP;

	#ifdef DEBUG
		inline u32  get_ref_count() NOEXP;
		inline u32  get_ref_count() const NOEXP;
	#endif

}; 


/*
	note : type should inherite from refcounter before using it with ref
*/
template<typename type> class ref {
	COMPILE_TIME_ASSERT(
		!std::is_base_of<refcounter, type>::value , 
		"class ref : " + typeid(type).name() + " type should inherite class refcounter !"
	);
	protected:
		type* object = nullptr;

	public:
		// constructor's
		ref() = default;
		ref(type* pointer) NOEXP;
		ref(ref<type> const& other) NOEXP; // copy
		ref(ref<type>&&      other) NOEXP; // move

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

}; // class ref end

// note: weak_ref/pointer have no ownership over memory
// note: this help solve circular dependency problem when --> A refer to B and B to A !! causing memory leak !
template<typename type> class weak_ref {

	// note: weak_ref points to object but no counted -> no counter update 


}; // class weak_ref end

#endif