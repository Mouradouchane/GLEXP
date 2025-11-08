#pragma once 

#ifndef CORE_CARRAY_HPP
#define CORE_CARRAY_HPP

#include <initializer_list>
#include "core/types.hpp"
#include "core/assert.hpp"
#include "core/memory/memory.hpp"


namespace core {

	/*
		c-style array as struct contain array variables and few static functions

		NOTE: - c_array is not safe , there's no check no type safety .
		      - designed for full control and access .
	*/
	template<typename type> struct c_array {
		// variables
		core::memory_allocator* allocator = nullptr;
		u32   count  = NULL;
		u32   size   = NULL;
		type* start  = nullptr;
		type* end    = nullptr;

		// operator's
		type& operator[](u32 index) {
			return *(this->start + index); 
		}
		
		// note: operator= performe a move operation
		core::c_array<type>& operator= (core::c_array<type>* other_array) {

			CORE_WARN_IF(this->start != nullptr, "core::c_array::operator= -> array elements begin wiped in the assignement process !");

			if (other_array != nullptr) {
				std::memcpy(this, other_array, sizeof(core::c_array<type>));
				std::memset(other_array, NULL, sizeof(core::c_array<type>));
			}
			else {
				std::memset(this, NULL, sizeof(core::array<type>));
			}

		}

		/*
			static function's
		*/ 

		static core::c_array<type> create(u32 elements_count, core::memory_allocator* allocator = nullptr) {

			core::c_array<type> new_array;

			new_array.count = elements_count;
			new_array.size  = sizeof(type) * elements_count;

			if (allocator == nullptr) {
				new_array.start = core::global_memory::allocate(new_array.size);
				CORE_WARN("core::c_array::create() -> core::c_array<{}> allocated using core::global_memory allocator !", typeid(type).name());
			}
			else {
				new_arary.start = allocator->allocate(new_array.size);
				new_array.allocator = allocator;
			}

			new_arary.end  = new_array.start + new_array.count;

			CORE_INFO("allocated core::c_array -> type:{} , address:&{} , size:{}", typeid(type).name(), &_array, _array->size);
			return new_array;
		}
		
		static void destroy(core::c_array<type>* _array) {
			CRASH_IF(_array == nullptr, "core::c_array::destroy() -> array is null-pointer !");

			if (_array->allocator == nullptr) {
				core::global_memory::deallocate(_array->start);
			}
			else {
				_array->allocator->deallocate(_array->start);
			}

			CORE_INFO("deallocated core::c_array -> type:{} , address:&{} , size:{}", typeid(type).name(), &_array, _array->size);
			memset(_array, 0, sizeof(core::c_array<type>));
		}

		static void clear(core::c_array<type>*_array) {
			CRASH_IF(_array == nullptr, "core::c_array::clear() -> null-pointer array !");

			memset(_array->start, 0, _array->size);
		}
	
		// todo: multi-threaded array fill
		static void fill(core::c_array<type>* _array, type const& fill_value) noexcept {
			CRASH_IF(_array == nullptr , "core::c_array::fill() -> array is null-pointer !");
			CRASH_IF(_array->start == nullptr , "core::c_array::fill() -> array memory is null-pointer !");

			std::fill<type>(_array->start , _array->end, fill_value);
		}

		// todo: multi-threaded copying
		static void copy(core::c_array<type> const& source, core::c_array<type>& destination) {
			CRASH_IF(source == nullptr || destination == nullptr, "core::c_array::copy(source={} , destination={}) : source or destination object is null-pointer !", &source, &destination);
			CRASH_IF(source->start == nullptr || destination->start == nullptr, "core::c_array::copy(source={}, destination={}) : source or destination memory is null-pointer !", &source, &destination);

			CORE_WARN_IF(source->size_ > destination->size_ , "core::c_array::copy(source={}, destination={}) : source array is bigger than the destination array !", &source, &destination);

			std::memcpy(destination->start , source->start , destination->size_);
		}

		// todo: multi-threaded moving
		static void move(core::c_array<type>* source, core::c_array<type>* destination) {
			// copy to destination
			std::memcpy(destination, source , sizeof(core::c_array<type>));
			// clear source
			std::memset(source, 0, sizeof(core::c_array<type>));
		}

		template<typename type> static void sort(
			core::c_array<type>& _array ,
			bool (*compare_function)(type const& a, type const& b)
		) noexcept {

			std::sort<type>(_array.start, _array.end, compare_function);
		}

	}; // struct c_array end

} // namespace core end

#endif