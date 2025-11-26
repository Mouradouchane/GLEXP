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
		u32 push_index   = NULL;
		u32 resize_value = CORE_DYNAMIC_ARRAY_DEFAULT_RESIZE_VALUE;

	private:
		dynamic_array(core::dynamic_array<type>&& other_array ) = delete;

	public:

		/*
			constructor's
		*/
		dynamic_array(u32 elements_count, u32 resize_value_, core::memory_allocator* _allocator = nullptr) 
			:core::array<type>(elements_count , _allocator)
		{	
			this->resize_value = resize_value_ || CORE_DYNAMIC_ARRAY_DEFAULT_RESIZE_VALUE;
		}

		// note : this copy elements !
		dynamic_array(type const& elements, u32 elements_count, u32 resize_value_, core::memory_allocator* _allocator = nullptr) 
			:core::array<type>(elements, elements_count , _allocator)
		{
			this->resize_value = resize_value_ || CORE_DYNAMIC_ARRAY_DEFAULT_RESIZE_VALUE;
		}

		// note : this copy everything
		dynamic_array(core::dynamic_array<type> const& other_array, core::memory_allocator* _allocator = nullptr)
			:core::array<type>(other_array.count_ , _allocator)
		{
			this->push_index   = other_array.push_index;
			this->resize_value = resize_value_ || CORE_DYNAMIC_ARRAY_DEFAULT_RESIZE_VALUE;

			// todo: make it multi-threaded !
			std::memcpy(this->start , other_array.start , this->size_);
		}

		/*
			destructor
		*/
		~dynamic_array() {
			core::array<type>::~array();
		}

		/*
			dynamic_array operator's
		*/

		// note: = operator performe move operation
		core::dynamic_array<type>& operator = (core::dynamic_array<type> const& other_array) {
			u32 array_size = sizeof(core::dynamic_array<type>);

			std::memcpy(this, other_array, array_size );
			std::memset(other_array,    0, array_size );

			return this;
		}

		/*
			note: using operator [] and push/pop function's -> could cause conflict and run-time bugs !
				  so use of them to avoid conflict's and bugs
		*/
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
			type* temp;
			u32   old_size = this->size_;

			this->count_ += this->resize_value;
			this->size_  = sizeof(type) * this->count_;

			if (this->allocator == nullptr) {
				temp = core::global_memory::allocate(this->size_);
			}
			else {
				temp = this->allocator->allocate(this->size_);
			}

			// copy elements to new memory
			std::memcpy(temp, this->start, old_size);

			this->start = temp;
			this->end   = this->start + this->size_;
		}

		/*
			note: using operator [] and push/pop function's -> could cause conflict and run-time bugs !
				  so use of them to avoid conflict's and bugs
		*/
		void push(type const& new_element) {

			if(this->push_index >= this->count_) this->resize();
			
			  this->push_index += 1;
			*(this->start + this->push_index) = new_element;
		}
		
		/*
			note: using operator [] and push/pop function's -> could cause conflict and run-time bugs !
				  so use of them to avoid conflict's and bugs
		*/
		void pop(bool call_destructor) {

			if (call_destructor && std::is_destructible<type>::value) {
				(this->start + index)->~type();
			}

			this->push_index -= (this->push_index) ? 1 : 0;
			std::memset(this->start + this->push_index, 0, sizeof(type));
		}

		template<typename type> static void sort(
			core::dynamic_array<type>& d_array,
			bool (*compare_function)(type const& a, type const& b)
		) noexcept {

			std::sort<type>(d_array.start, d_array.end, compare_function);
		}

	}; // class dynamic_array end

	typedef struct obj {
		u32 a;
		u32 b;
	};

	core::memory_heap heap1(32 MB);

	core::dynamic_array<obj>  vobj(255, 64, &heap1);
	core::dynamic_array<obj> vobj2(255, 64 , nullptr);

} // namespace core end

#endif