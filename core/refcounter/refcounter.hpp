#pragma once 

#ifndef CORE_REFCOUNTER_HPP
#define CORE_REFCOUNTER_HPP

#include "core/macros.hpp"
#include "core/types.hpp"

#include <atomic>

/*

		here is a class for -> "intrusive refernce counter"
		used to keep track of objects life-time automatically

*/

struct refcounter {
	std::atomic<u32> count;
};

template<typename type> class ref {

	protected:
		refcounter counter;
		type*      data;

	public:


}; // class ref end


#endif