#pragma once 

#ifndef CORE_ARRAY_HPP
#define CORE_ARRAY_HPP

#include <cstdarg>
#include "core/types.hpp"
#include "core/assert.hpp"
#include "core/memory/memory.hpp"

namespace core {

	/*
		class for fixed size array , array memory allocated on the heap not in stack
	*/
	template<typename type> class array {
	private:
		// note: disabled constructors/operators
		array(core::array&& other_array)           = delete;
		array(const core::array& other_array)      = delete;
		array& operator=(const array& other_array) = delete;
		// ====================================

	private:
		core::memory_allocator* allocator = nullptr;

		u32   size__ = NULL;
		type* start  = nullptr;
		type* end    = nullptr;

	public:
		/*
			constructor's
		*/ 
		array(u32 elements_count, memory_allocator* memory_allocator = nullptr) 
			:allocator(memory_allocator) 
		{
			if (this->allocator == nullptr) {
				this->start = (type*)core::global_memory::allocate(sizeof(type)*elements_count , memory_usage::buffers);
			}else {
				this->start = (type*)this->allocator->allocate(sizeof(type)*elements_count);
			}
			this->end    = this->start + elements_count;
			this->size__ = this->end - this->start;
		}
		
		array(type const& elements , u32 elements_count , memory_allocator* memory_allocator = nullptr )
			:allocator(memory_allocator) 
		{
			if (this->allocator == nullptr) {
				this->start = (type*)core::global_memory::allocate(sizeof(type)*elements_count , memory_usage::buffers);
			}else {
				this->start = (type*)this->allocator->allocate(sizeof(type)*elements_count);
			}
			this->end    = this->start + elements_count;
			this->size__ = this->end - this->start;

			// copy elements to array
			for (u32 i = 0; i < this->size__; i++) {
				*(this->start + i) = *(elements + i);
			}
		}

		/*
			destructor
		*/
		~array() {

			// destroy all elements in array
			if (this->start != nullptr) {
				for (type* ptr = this.start; ptr != this.end; ptr++) {
					ptr->~type();
				}
			}

			// free memory
			if (this->allocator != nullptr) {
				this->allocator->deallocate(this->start);
			}
			else {
				core::global_memory::deallocate(this->allocator);
			}

			this->start = nullptr;
			this->end   = nullptr;
		}

		// array public functions
		type* begin() noexcept;
		type* end()   noexcept;

		type* get(u32 index);
		void  set(u32 index, type const& new_element);

		void remove(u32 index , bool call_destructor);

		u32 size() noexcept; // elements count
		u64 size_of() noexcept; // size in bytes
		f64 size_of(memory_unit unit) noexcept; // size in kb,mb,gb

		// operator's
		type& operator[](u32 index);

		// array static public functions
		static void copy(core::array& source , core::array& destination);
		static void move(core::array& source , core::array& destination);
	
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

	template<typename type> void array<type>::remove(u32 index , bool call_destructor) {
		CRASH_IF(index >= this->size__, "array.remove: index out of range !");

		if (call_destructor) {
			// todo: find a better way to handle "error here"
			this->start[index].~type();
		}

		*(this->start + index) = type();
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