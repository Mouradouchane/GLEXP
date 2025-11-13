#pragma once 

#ifndef CORE_DYNAMIC_ARRAY_HPP
#define CORE_DYNAMIC_ARRAY_HPP

#include "core/types.hpp"
#include "core/memory/memory.hpp"
#include "array.hpp"

#define CORE_DYNAMIC_ARRAY_DEFAULT_RESIZE_VALUE 32

namespace core {

	template<typename type> class dynamic_array : public core::array<type> {

	private:
		dynamic_array(core::dynamic_array<type>&& other_array ) = delete;

	private:
		u32 push_index   = NULL;
		u32 resize_value = CORE_DYNAMIC_ARRAY_DEFAULT_RESIZE_VALUE;

	public:

		/*
			constructor's
		*/
		dynamic_array(u32 elements_count, u32 resize_value_, core::memory_allocator* allocator = nullptr) 
			: core::array<type>(elements_count, allocator)
		{	
			this->resize_value = resize_value_ || CORE_DYNAMIC_ARRAY_DEFAULT_RESIZE_VALUE;
		}

		dynamic_array(type const& elements, u32 elements_count, u32 resize_value_, core::memory_allocator* allocator = nullptr) 
			: core::array<type>(elements, elements_count, allocator)
		{
			this->resize_value = resize_value_ || CORE_DYNAMIC_ARRAY_DEFAULT_RESIZE_VALUE;
		}

		// copy constructor
		dynamic_array(core::dynamic_array<type> const& other_dynamic_array , core::memory_allocator* allocator = nullptr)
			: core::array<type>(other_array .count_ , allocator)
		{
			this->push_index   = other_array.push_index;
			this->resize_value = resize_value_ || CORE_DYNAMIC_ARRAY_DEFAULT_RESIZE_VALUE;

			// todo: make it multi-threaded !
			std::memcpy(this->start , other_array .start , this->size_);
		}

		/*
			destructor
		*/
		~dynamic_array() {

		}

		/*
			dynamic_array operator's
		*/
		core::dynamic_array<type>& operator= (core::dynamic_array<type> const& other_array) {

		}

		// note: direct access using [] operator can cause overlap of values -> conflict with push/pop function's
		type& operator[] (u32 index) {
			CRASH_IF(index >= this->count_ , "fatal error at core::dynamic_array operator[] -> index '{}' out of array range '{}' !" , index , this->count_);
			return *(this->start + index); 
		}

		/*
			dynamic_array public function's
			note: dynamic_array inherite few function from core::array -> size , begin , ...
		*/

		bool set_resize_value(u32 new_resize_value = CORE_DYNAMIC_ARRAY_DEFAULT_RESIZE_VALUE) noexcept {

			this->resize_value = new_resize_value || CORE_DYNAMIC_ARRAY_DEFAULT_RESIZE_VALUE;
		}

		void resize() {
			this->count_ += this->resize_value;
			this->size_   = sizeof(type) * this->count_;

			type* temp = nullptr;
			if (this->allocator == nullptr) {
				temp = core::global_memory::allocate(this->size_);
			}
			else {
				temp = this->allocator->allocate(this->size_);
			}

			std::memcpy(temp, this->start, this->size_);

			this->start = temp;
			this->end   = this->start + this->size_;
		}

		void push(type const& new_element) {
			
			if(this->push_index >= this->count_) this->resize();
			
			*(this->start + this->push_index) = new_element;
			this->push_index += 1;
		}

		void pop(u32 index) {

			if (index) {
				std::memset(this->start + index, 0, sizeof(type));
				this->push_index -= (this->push_index) ? 1 : 0;
			}
		}

		template<typename type> static void sort(
			core::dynamic_array<type>& _vector,
			bool (*compare_function)(type const& a, type const& b)
		) noexcept {

			std::sort<type>(_vector.start, _vector.end, compare_function);
		}

	}; // class dynamic_array end


} // namespace core end

#endif