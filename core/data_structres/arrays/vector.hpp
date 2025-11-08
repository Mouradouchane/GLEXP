#pragma once 

#ifndef CORE_VECTOR_HPP
#define CORE_VECTOR_HPP

#include "core/types.hpp"
#include "core/memory/memory.hpp"
#include "array.hpp"

namespace core {

	template<typename type> class vector : public core::array<type> {

	private :
		vector(core::vector<type>&& other_array) = delete;

	public:

		/*
			constructor's
		*/
		vector(u32 elements_count, core::memory_allocator* allocator) {

		}

		vector(const type* elements, u32 elements_count, u32 resize_value, core::memory_allocator* allocator) {

		}

		vector(const core::vector<type>& other_array) {

		}


		/*
			destructor
		*/
		~vector() {

		}

		/*
			vector operator's
		*/
		core::vector<type>& operator= (core::vector<type> const& other_array) {

		}

		type& operator[] (u32 index) {

		}

		/*
			vector function's
		*/
		void resize(u32 resize_value = NULL) {


		}

		void push(type const& new_element) {
				

		}

		template<typename type> static void sort(
			core::vector<type>& _vector,
			bool (*compare_function)(type const& a, type const& b)
		) noexcept {

			std::sort<type>(_vector.start, _vector.end, compare_function);
		}

	}; // class vector end


} // namespace core end

#endif