#pragma once 

#ifndef CORE_ARRAY_HPP
#define CORE_ARRAY_HPP

#include <algorithm>
#include <cstring>
#include <type_traits>

#include "core/types.hpp"
#include "core/assert.hpp"
#include "core/memory/memory.hpp"

namespace core {

	/*
		class for fixed size array , array memory allocated on the heap not in stack
	*/
	template<typename type> class array {

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
		
		array(type const& elements , u32 elements_count , memory_allocator* memory_allocator = nullptr)
			:allocator(memory_allocator)
		{
			if(this->allocator == nullptr) {
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

		// todo: implement copy/move constructors
		core::array(core::array<type>&& other_array)           = delete;
		core::array(const core::array<type>& other_array)      = delete;
		// ====================================
		/*
			destructor
		*/
		~array() {

			// destroy all elements in array
			if (this->start != nullptr && std::is_destructible<type>::value) {
				for (type* ptr = this.start; ptr != this.end; ptr++) {
					ptr->~type();
				}
			}

			// free memory
			if (this->allocator != nullptr) {
				this->allocator->deallocate(this->start);
			}
			else {
				core::global_memory::deallocate(this->start);
			}

			this->start  = nullptr;
			this->end    = nullptr;
			this->size__ = NULL;
		}

		/*
			array public functions
		*/
		type& get(u32 index) {
			CRASH_IF(index >= this->size__ , "array.get(index = {}): is out of array range" , index);
			return (this->start + index);
		}

		void set(u32 index, type& new_element) {
			CRASH_IF(index >= this->size__ , "array.set(index = {}): index out of array range" , index);

			*(this->start + index) = new_element;
		}

		type const* pointer() noexcept{
			return this->start;
		}

		type* begin() noexcept{
			return this->start;
		}

		type* end() noexcept{
			return this->end;
		}

		void clear() {
			std::memset((void*)this->start, 0, sizeof(type) * this->size__);
		}

		u32 size() noexcept { 
			return this->size__;
		}

		// size in bytes
		u64 size_of() noexcept {  
			return u64(this->size__ * sizeof(type));
		}

		f64 size_of(memory_unit unit) noexcept {
			
			switch (unit) {
				case memory_unit::kb: return BYTE_TO_KB(this->size__ * sizeof(type));
				case memory_unit::mb: return BYTE_TO_MB(this->size__ * sizeof(type));
				case memory_unit::gb: return BYTE_TO_GB(this->size__ * sizeof(type));

				default: return f64(this->size__ * sizeof(type));
			}
		}


		/*
			operator's
		*/ 
		type& operator[](u32 index) { // warning no checks here use get() for safe access
			CRASH_IF(index >= this->size__ , "array[{}] : index out of array range" , index);
			return *(this->start + index); 
		}

		// note: operator= do a move operation
		core::array<type>& operator= (core::array<type> const& other_array) {
			CORE_WARN("core::array {} = array{}) : all the elements in the array discarded in the process !" , &this , &other_array);
			
			this->start  = other_array.start;
			this->size__ = other_arary.size__;
			this->end    = this->start + this->size__;

			other_array.start  = nullptr;
			other_array.end    = nullptr;
			other_array.size__ = NULL;
		}


	public:
		/*
				array static public functions
		*/ 

		// todo : add option for multi-threaded copying later
		static void copy(core::array<type>& source, core::array<type>& destination) {
			CORE_WARN(destination.size__ < source.size__ , "core::array::copy(source={}, destination={}) : size of destination is smaller than the source !");
			CRASH_IF(source.start == nullptr || destination.start == nullptr, "core::array::copy(source={}, destination={}) : source or destination memory is null", &source, &destination);
		
			for (u32 i = 0; i < destination.size__; i++) {
				*(destination.start + i) = *(source.start + i);
			}
		}

		static void move(core::array<type>& source, core::array<type>& destination) {
			if (destination.start != nullptr) {
				if (destination.allocator == nullptr) {
					core::global_memory::deallocate(destination.start);
				}
				else destination.allocator.deallocate(destination.start);
				
				CORE_WARN("core::array:move(source={}, destination={}) : all the elements in the destination array discarded in the process !" , &source , &destination);
			}

			destination.start  = source.start;
			destination.size__ = source.size__;
			destination.end    = destination.start + destination.size__;

			source.start  = nullptr;
			source.end    = nullptr;
			source.size__ = NULL;
		}

		// todo : add option for multi-threaded copying later
		static void fill(array<type>& _array, type const& fill_value) noexcept {

			for (type* ptr = _array.start; ptr != _array.end; ptr++) {
				*ptr = fill_value;
			}

		}

		template<typename type> static inline void array<type>::sort(
			array<type>& _array,
			bool (*compare_function)(type const& a, type const& b)
		) {
			std::sort<type>(this->start, this->end, compare_function);
		}

	}; // class array end

} // namespace core end

#endif