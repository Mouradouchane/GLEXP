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
		class for fixed count array , array memory allocated on the heap not in stack
	*/
	template<typename type> class array {

	private:
		core::memory_allocator* allocator;
		u32   size_  = NULL;
		u32   count_ = NULL;
		type* start  = nullptr;
		type* end    = nullptr;

	private :
		array(core::array<type>&& other_array) = delete;

	public:
		/*
			constructor's
		*/ 
		array(u32 elements_count , core::memory_allocator* _allocator = nullptr)
			:allocator(_allocator), :count_(elements_count), :size_(elements_count * sizeof(type))
		{

			if (this->allocator == nullptr) {
				this->start = (type*)(core::global_memory::allocate(this->size_));
			}
			else {
				this->start = (type*)(this->allocator->allocate(this->size_));
			}

			this->end = this->start + this->count_;

			CORE_INFO("core::array<{}> allocated with size:{} at address:&{} ", typeid(type).name(), &this , this->size_);
		}
		
		array(type const& elements, u32 elements_count, core::memory_allocator* _allocator = nullptr)
			:allocator(_allocator), :count_(elements_count), :size_(elements_count * sizeof(type))
		{

			if (this->allocator == nullptr) {
				this->start = (type*)core::global_memory::allocate(this->size_);
			}
			else {
				this->start = (type*)this->allocator->allocate(this->size_);
			}

			this->end = this->start + this->count_;

			// todo: make it multi-threaded
			// copying elements to array
			std::memcpy(this->start, &elements, this->size_);

			CORE_INFO("allocated core::array<{}> -> address:&{} , size:{}", typeid(type).name(), &this , this ->size_);
		}

		// copy constructor
		array(core::array<type> const& other_array, core::memory_allocator* _allocator = nullptr) 
			: allocator(_allocator)
		{
			this->size_ = other_array.size_;

			// allocate array memory 
			if (this->allocator == nullptr) {
				this->start = (type*)core::global_memory::allocate(sizeof(type) * this->size_);
				CORE_WARN("core::array<{}> allocated using core::global_memory allocator .", typeid(type).name());
			}
			else {
				this->start = (type*)this->allocator->allocate(sizeof(type) * this->size_);
			}

			// todo: make it multi-threaded !
			std::memcpy(this->start , other_array.start , this->size_);

			CORE_INFO("allocated core::array<{}> -> address:&{} , size:{}", typeid(type).name(), &this , this ->size_);
		}

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

			// free array memory
			if (this->allocator != nullptr) {
				this->allocator->deallocate(this->start);
			}
			else {
				core::global_memory::deallocate(this->start);
			}

			CORE_INFO("deallocated core::array<{}> at address:&{} with size:{}", typeid(type).name(), &this, this->size_);
		}
		
		/*
			array public functions
		*/
		type& get(u32 index) {
			CRASH_IF(index >= this->count_ , "error at core::array.get() -> index '{}' out of array range '{}' !" , index , this->count_);

			return (this->start + index);
		}

		void set(u32 index, type& new_element) {
			CRASH_IF(index >= this->count_ , "error at core::array.set() -> index '{}' out of array range '{}' !" , index , this->count_);

			*(this->start + index) = new_element;
		}

		type* begin() noexcept{
			return this->start;
		}

		type* end() noexcept{
			return this->end;
		}

		void clear() {
			std::memset(this->start, 0, this->size_);
		}

		u32 count() noexcept { 
			return this->count_;
		}

		// count in bytes
		u32 size() noexcept {  
			return this->size_;
		}

		f64 size(memory_unit unit) noexcept {
			
			switch (unit) {
				case memory_unit::kb : return BYTE_TO_KB(this->size_);
				case memory_unit::mb : return BYTE_TO_MB(this->size_);
				case memory_unit::gb : return BYTE_TO_GB(this->size_);

				default: return f64(this->size_);
			}

		}


		/*
			operator's
		*/ 
		type& operator[](u32 index) {
			CRASH_IF(index >= this->count_ , "error at core::dynamic_array operator[] : index '{}' out of array range '{}' !" , index , this->count_);
			return *(this->start + index); 
		}

		// note: operator = performe a move operation
		core::array<type>& operator= (core::array<type>& other_array) {
			CORE_WARN_IF(this->start != nullptr, "core::array<{}> &{} : array elements begin wiped in the assignement process !", typeid(type).name(), this);
			
			std::memcpy(this, other_array, sizeof(core::array<type>));
			std::memset(other_array, NULL, sizeof(core::array<type>));
		}

	public:
		/*
				array static public functions
		*/ 

		// todo : add multi-threaded copying !
		static void copy(core::array<type> const& source, core::array<type>& destination) {
			CRASH_IF(source.start == nullptr || destination.start == nullptr, "core::array::copy(source={}, destination={}) : source or destination memory is null-pointer !", &source, &destination);
			CORE_WARN_IF(source->size_ > destination->size_ , "core::array::copy(source={}, destination={}) : source array is bigger than the destination array !", &source, &destination);
		
			std::memcpy(destination->start , source->start , destination->size_);
		}

		static void move(core::array<type>& source, core::array<type>& destination) {
			// copy to destination
			std::memcpy(destination, source , sizeof(core::array<type>));
			// zero the source
			std::memset(source, 0, sizeof(core::array<type>));
		}

		// todo : add option for multi-threaded copying later
		static void fill(array<type>& _array, type const& fill_value) noexcept {
			CRASH_IF(_array->start == nullptr , "core::array::fill() -> array memory is null-pointer !");

			std::fill<type>(_array->start , _array->end, fill_value);
		}

		template<typename type> static inline void array<type>::sort(
			core::array<type>& _array,
			bool (*compare_function)(type const& a, type const& b)
		) noexcept 
		{
			std::sort<type>(_array.start, _array.end, compare_function);
		}

	}; // class array end


} // namespace core end

#endif