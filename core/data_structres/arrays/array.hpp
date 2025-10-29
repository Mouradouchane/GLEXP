#pragma once 

#ifndef CORE_ARRAY_HPP
#define CORE_ARRAY_HPP

#include <initializer_list>
#include "core/types.hpp"
#include "core/assert.hpp"
#include "core/memory/memory.hpp"

namespace core {

	/*
		class for fixed size array allocated on the memory_heap
		with few vars and functions
	*/
	template<typename type> class array {
	private:
		// note: disabled constructors/operators
		array(const array& other_array) = delete;
		array(array&& other_array) = delete;
		array& operator=(const array& other_array) = delete;
		// ====================================

	private:
		u32   len__  = NULL;
		u32   size__ = NULL;
		type* start  = nullptr; // global_memory start
		type* end    = nullptr; // global_memory end

	public:
		// constructor's / destructor
		 array(u32 elements_count);
		 array(u32 elements_count, std::initializer_list<type> const& elements);
		~array();

		// function's
		type* begin() noexcept;
		type* end()   noexcept;

		type* get(u32 index);
		void  set(u32 index, type const& new_element);

		/*
			note: to call element destructor before remove use destroy
		*/
		void remove(u32 index);
		void destroy(u32 index);

		u32 size() noexcept; // elements count
		u64 size_of(void) noexcept; // size in bytes
		f64 size_of(memory_unit unit) noexcept; // size in kb,mb,gb

		// operator's
		type& operator[](u32 index);

		// few static functions for array
		static void fill(array<type>& _array, type const& fill_value) noexcept;
		static void sort(
			array<type>& _array,
			bool (*compare_function)(type const& a, type const& b)
		);
	};

} // namespace core end

/*
	=====================================
			implemenation
	=====================================
*/

namespace core {
	/*
		constructor's
	*/
	template<typename type>	 array<type>::array(u32 elements_count) {
		CRASH_IF(!elements_count, "array: 0 size array not allowed !");

		// todo: implement + "global_memory allocation solution"

	}

	template<typename type>	 array<type>::array(
		u32 elements_count, std::initializer_list<type> const& elements
		) {
		CRASH_IF(!elements_count, "array: 0 size array not allowed !");
		CRASH_IF(elements_count < elements.size(), "array: elements count larger than the array !");

		// todo: implement + "global_memory allocation solution"

	}

	/*
		destructor
	*/
	template<typename type>	array<type>::~array() {

		// destroy all array elements
		for (type* ptr = this.start; ptr != this.end; ptr += 1) {
			ptr->~type();
		}
	}

	/*
		array function's
	*/
	template<typename type> type* array<type>::begin() noexcept {
		return this->start;
	}

	template<typename type> type* array<type>::end() noexcept {
		return this->end;
	}

	template<typename type> void array<type>::set(u32 index, type const& new_element) {
		CRASH_IF(index >= this->size__, "array.set: index out of array range !");

		*(this->start + index) = new_element;
		this->len__ += 1;
	}

	template<typename type> void array<type>::remove(u32 index) {
		CRASH_IF(index >= this->size__, "array.remove: index out of range !");

		*(this->start + index) = type();
		this->len__ -= (this->len__ > 0) ? this->len__ - 1 : 0;
	}

	template<typename type> void array<type>::destroy(u32 index) {
		CRASH_IF(index >= this->size__, "array.destroy: index out of range !");

		(this->start + index)->~type(); // destroy
		(this->start + index) = type();
		this->len__ -= (this->len__ > 0) ? this->len__ - 1 : 0;
	}

	template<typename type> u32 array<type>::size() noexcept {
		return this->size__;
	}

	template<typename type> u64 array<type>::size_of(void) noexcept {
		return u64(this->size__ * sizeof(type));
	}

	template<typename type> f64 array<type>::size_of(memory_unit unit) noexcept {
		switch (unit) {
			case memory_unit::kb: return BYTE_TO_KB(this->size__ * sizeof(type));
			case memory_unit::mb: return BYTE_TO_MB(this->size__ * sizeof(type));
			case memory_unit::gb: return BYTE_TO_GB(this->size__ * sizeof(type));

			default: return f64(this->size__ * sizeof(type));
		}
	}

	/*
		array operator's
	*/
	template<typename type> type& array<type>::operator[](u32 index) {
		CRASH_IF(index >= this->size__, "array[]: index out of array range !");

		return *(this->start + index);
	}

	/*
		array static function's
	*/

	template<typename type> static void array<type>::fill(
		array<type>& _array, type const& fill_value
	) noexcept {

		for (type* ptr = _array.start; ptr != _array.end; ptr += 1) {
			*ptr = fill_value;
		}

	}

	// todo: implement sort for core::array
	template<typename type> static void array<type>::sort(
		array<type>& _array,
		bool (*compare_function)(type const& a, type const& b)
	) {
		CORE_ASSERT(true, "array::sort() is used , but not implement yet !");
	}

} // namespace core end

#endif