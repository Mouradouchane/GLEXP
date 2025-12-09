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
		u32 push_index   = 0;
		u32 resize_value = CORE_DYNAMIC_ARRAY_DEFAULT_RESIZE_VALUE;

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
			:core::array<type>(other_array , _allocator)
		{
			this->push_index   = other_array.push_index;
			this->resize_value = other_array.resize_value ? other_array.resize_value : CORE_DYNAMIC_ARRAY_DEFAULT_RESIZE_VALUE;

			// todo: make it multi-threaded !
			// std::memcpy(this->begin_ , other_array.begin_ , this->size_);
		}

		/*
			destructor
		*/
		~dynamic_array() = default;

		/*
			dynamic_array operator's
		*/

		// note: equal operator performe move ownership operation
		core::dynamic_array<type>& operator = (core::dynamic_array<type> const& other_array) {
			if (this == &other_array) return *this;

			/*
			u32 array_size = sizeof(core::dynamic_array<type>);
			std::memcpy(this, other_array, array_size );
			std::memset(other_array,    0, array_size );
			*/

			core::array<type>::operator=(other_array);
			this->push_index   = other.push_index;
			this->resize_value = other.resize_value;

			return *this;
		}

		/*
			note: using operator [] and push/pop function's -> could cause conflict and run-time bugs !
				  so use of them to avoid conflict's and bugs
		*/
		type& operator[] (u32 index) {
			CRASH_IF(index >= this->count_ , "core::dynamic_array::operator[] : index '{}' out of array range '{}' !" , index , this->count_);
			return this->begin_[index]; 
		}

		/*
			dynamic_array public function's
			note: dynamic_array inherite few function from core::array -> size , begin , ...
		*/

		void set_resize_value(u32 new_resize_value = CORE_DYNAMIC_ARRAY_DEFAULT_RESIZE_VALUE) noexcept {
			this->resize_value = new_resize_value ? new_resize_value : CORE_DYNAMIC_ARRAY_DEFAULT_RESIZE_VALUE;
		}

		void resize() {
			type* new_buffer;
			u32   old_count = this->count_;
			u32   old_size  = this->size_;

			// update array variables
			this->count_ += this->resize_value;
			this->size_   = sizeof(type) * this->count_;

			// allocate new memory
			if (this->allocator) {
				new_buffer = (type*)this->allocator->allocate(this->size_);
			}
			else {
				new_buffer = (type*)core::global_memory::allocate(this->size_);
			}

			// move elements to new memory
			if constexpr(std::is_trivially_copyable<T>::value) {
				std::memmove(new_buffer, this->begin_, old_size);
			}
			else {
				// move elements to new memory
				for (u32 i = 0; i < old_count; i++) {
					new_buffer[i] = this->begin_[i];
				}

				// destroy old elements
				for (u32 i = 0; i < old_count; i++) {
					this->begin_[i].~T();
				}
			}

			// deallocate old memory
			if (this->allocator) {
				this->allocator->deallocate(this->begin_);
			} 
			else {
				core::global_memory::deallocate(this->begin_);
			}

			// update variables
			this->begin_ = new_buffer;
			this->end_   = this->begin_ + this->count_;
		}

		/*
			note: using operator [] and push/pop function's in the same time could cause conflict's and overlap's !
				  avoid using both of them at once to avoid conflict's and run-time bugs !
		*/
		void push(type const& new_element) {

			// if resize is needed
			if(this->push_index >= this->end_) this->resize();
			
			// push then update counter
			this->begin_[this->push_index] = new_element;
			this->push_index += 1;
		}
		
		/*
			note: using operator [] and push/pop function's in the same time could cause conflict's and overlap's !
			avoid using both of them at once to avoid conflict's and run-time bugs !
		*/
		void pop(bool call_destructor) {

			if (call_destructor && constexpr(std::is_trivially_destructible<type>::value) ){
				this->begin_[index].~type();
			}
				
			std::memset(this->begin_[this->push_index], 0, sizeof(type));

			this->push_index -= (this->push_index) ? 1 : 0;	
		}


	}; // class dynamic_array end

} // namespace core end

#endif