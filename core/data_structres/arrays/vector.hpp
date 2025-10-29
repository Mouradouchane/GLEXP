#pragma once 

#ifndef CORE_VECTOR_HPP
#define CORE_VECTOR_HPP

#include <initializer_list>

#include "core/types.hpp"
#include "core/memory/memory.hpp"
#include "array.hpp"

namespace core {

	template<typename type> class vector : public core::array<type> {

		private:
			/*
				todo : implement move / copy for core::vector
			*/ 
			vector(const vector& other_array) = delete;
			vector(vector&& other_array)      = delete;
			vector& operator= (const vector& other_array) = delete;
			// ====================================

		private:
			u32 push_index = nullptr;

		public:
			// constructor's / destructor
			vector(u32 elements_count);
			vector(
				u32 elements_count,
				std::initializer_list<type> const& elements
			);
			~vector();

			// vector public function's
			void push(type const& new_element);
			void resize(u32 elements_count);

	}; // class vector end


	/*
		====================================
			vector implementation
		====================================
	*/

	// constructor's
	template<typename type>	vector<type>::vector(u32 elements_count) {
		CRASH_IF(!elements_count, "array: 0 size array not allowed !");

		// todo: implement + "global_memory allocation solution"

	}

	template<typename type>	vector<type>::vector(
		u32 elements_count,
		std::initializer_list<type> const& elements
	) {
		CRASH_IF(!elements_count, "array: 0 size array not allowed !");

		// todo: implement + "find a solution for custom memory allocators"

	}

	/*
		destructor
	*/
	template<typename type>	vector<type>::~vector() {

		// destroy all array elements
		for (type* ptr = this.start; ptr != this.end; ptr += 1) {
			ptr->~type();
		}

	}

	/*
		====================================
				vector function's
		====================================
	*/

	template<typename type>	void vector<type>::push(type const& new_element) {

		// if resize required
		if ((this->start + this->push_index) >= this->end) {
			this->resize(this->size__);
		}

		// push new element
		*(this->start + this->push_index) = new_element;
		this->push_index += 1;

	}

	template<typename type>	void vector<type>::resize(u32 elements_count) {

		this->size__ += elements_count;
		// todo: resize array using "global_memory allocator"
		this->start = nullptr;
		this->end = this->start + this->size__;

		// todo: copy elements to the new global_memory

	}

} // namespace core end

#endif