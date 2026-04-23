#pragma once 

#ifndef CORE_REFCOUNTER_HPP
#define CORE_REFCOUNTER_HPP

#include <concepts>
#include <atomic>

#include "core/types.hpp"

// inject this macro at the end of your class !
// to make it acceptable by "ref counter classes"
#define INJECT_REF_COUNTER(THIS_TYPE) \
		public: \
			using __is_refcounter_type__ = void; \
		protected : \
			template<refcounted_type THIS_TYPE> friend class shared_ref; \
			template<refcounted_type THIS_TYPE> friend class weak_ref; \
			template<refcounted_type THIS_TYPE> friend class strong_ref; \
			std::atomic<u32> __strong__ = 0; \
			std::atomic<u32>  __weak__  = 0;


// few "rules and demands" here for any types we gonna be used with refs 
template<typename type> concept refcounted_type = requires(type t) {

	// rule 1 : inject this macro in your type/class --> INJECT_REF_COUNTER(type)
	// why ? that macro add counter and make your type "compatible & usable & managable" by our refs classes.
	typename type::__is_refcounter_type__;

	// rule 2 : your type must have destructor "~type() NOEXP" 
	requires std::destructible<type>;
	requires std::is_nothrow_destructible_v<type>;

	// rule 3 [optional] : make destructor of your type virtual , if you have any inheritance !
	// to make sure the right destructor begin called not the wrong one !
	// requires std::has_virtual_destructor_v<type>;

};

#endif