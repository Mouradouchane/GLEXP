#pragma once 

#ifndef CORE_DYNAMIC_ARRAY_HPP
#define CORE_DYNAMIC_ARRAY_HPP

#include "core/types.hpp"
#include "core/memory/memory.hpp"
#include "array.hpp"

#define CORE_DYNAMIC_ARRAY_DEFAULT_RESIZE_VALUE 32

namespace core {

	template<typename type> class dynamic_array : protected core::array<type> {

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
			this->resize_value = resize_value_ ? resize_value_ : CORE_DYNAMIC_ARRAY_DEFAULT_RESIZE_VALUE;
		}

		// note : this copy elements !
		dynamic_array(const type* elements, u32 elements_count, u32 resize_value_, core::memory_allocator* _allocator = nullptr) 
			:core::array<type>(elements, elements_count , _allocator)
		{
			this->resize_value = resize_value_ ? resize_value_ : CORE_DYNAMIC_ARRAY_DEFAULT_RESIZE_VALUE;
		}

		// note : this copy everything
		dynamic_array(core::dynamic_array<type> const& other_array, core::memory_allocator* _allocator = nullptr)
			:core::array<type>(other_array , _allocator)
		{
			this->push_index   = other_array.push_index;
			this->resize_value = other_array.resize_value ? other_array.resize_value : CORE_DYNAMIC_ARRAY_DEFAULT_RESIZE_VALUE;
		}

		/*
			destructor
		*/
		~dynamic_array() = default;


		/*
			dynamic_array operator's
		*/

		// note: performe copy operation
		// note: copying discard old elements
		core::dynamic_array<type>& operator = (core::dynamic_array<type> const& array_to_copy) {
			if (this == &array_to_copy) return *this;
			
			if(array_to_copy.begin_){

				// update variables
				this->push_index   = array_to_copy.push_index;
				this->resize_value = array_to_copy.resize_value;

				// allocate memory if not allocated yet
				if (this->begin_ == nullptr) {

					if (!this->allocator) {
						this->allocator = array_to_copy.allocator;	 
					}

					core::dynamic_array<type>::allocate(*this, array_to_copy.count_);
					
					this->count_ = array_to_copy.count_;
					this->size_  = array_to_copy.size_;
					this->end_   = this->begin_ + this->count_;
				}

				// copying elements
				if constexpr(std::is_trivially_copyable<type>::value) {
					u32 copy_size = (array_to_copy.size_ > this->size_) ? this->size_ : array_to_copy.size_;
					std::memcpy(this->begin_, array_to_copy.begin_, copy_size);
				}
				else {
					u32 copy_count = (array_to_copy.count_ > this->count_) ? this->count_ : array_to_copy.count_;
					// todo: multi-threaded copying
					for (u32 i = 0; i < copy_count; i++) {
						this->begin_[i] = array_to_copy.begin_[i];
					}
				}

			}

			return *this;
		}

		// note: performe move ownership operation
		core::dynamic_array<type>& operator = (core::dynamic_array<type>&& array_to_move) {
			if (this == &array_to_move) return *this;

			if (this->begin_ != nullptr) {

				// destruct current elements
				if constexpr (!std::is_trivially_destructible<type>::value) {
					for (type* p = this->begin_; p != this->end_; ++p) {
						p->~type();
					}
				}

				// deallocate current memory
				if (this->allocator) {
					this->allocator->deallocate(this->begin_);
				} 
				else {
					core::global_memory::deallocate(this->begin_);
				}
			}

			// move ownership
			this->allocator = array_to_move.allocator;
			this->begin_    = array_to_move.begin_;
			this->end_      = array_to_move.end_;
			this->size_     = array_to_move.size_;
			this->count_    = array_to_move.count_;

			// clear other array 
			array_to_move.allocator = nullptr;
			array_to_move.begin_    = nullptr;
			array_to_move.end_      = nullptr;
			array_to_move.size_     = 0;
			array_to_move.count_    = 0;

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
			if constexpr(std::is_trivially_copyable<type>::value) {
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
			if((this->begin_ + this->push_index) >= this->end_) this->resize();
			
			// push then update counter
			this->begin_[this->push_index] = new_element;
			this->push_index += 1;
		}
		
		/*
			note: using operator [] and push/pop function's in the same time could cause conflict's and overlap's !
			avoid using both of them at once to avoid conflict's and run-time bugs !
		*/
		void pop(bool call_destructor) {

			if (call_destructor) {
				if (constexpr(std::is_trivially_destructible<type>::value)) {
					this->begin_[push_index].~type();
				}
			}
				
			std::memset(&this->begin_[this->push_index], 0, sizeof(type));

			this->push_index -= (this->push_index) ? 1 : 0;	
		}
	
	private:
		/*
			few private helper functions
		*/ 

		static inline void deallocate(core::dynamic_array<type>& array_, bool destruct_elements) {

			if (!array_.begin_) {
				array_.begin_  = nullptr;
				array_.end_    = nullptr;
				array_.count_  = 0;
				array_.size_   = 0;
				array_.push_index = 0;
				
				return;
			}

			// destruct elements if destructable 
			if (destruct_elements && constexpr(!std::is_trivially_destructible<type>::value) ) {
				
				// todo: maybe multi-threaded destruction if possible !!!
				for (type* obj = array_.begin_; obj != (array_.begin_ + array_.count_); obj++) {
					obj->~type();
				}
			}

			// deallocate array memory
			if(array_.begin_){

				if (array_.allocator) {
					array_.allocator->deallocate(array_.begin_);
				}
				else {
					core::global_memory::deallocate(array_.begin_);
				}
			}

			array_.begin_  = nullptr;
			array_.end_    = nullptr;
			array_.count_  = 0;
			array_.size_   = 0;
			array_.push_index = 0;
		}

		static inline void allocate(core::dynamic_array<type>& array_ , u32 new_elements_count) {

			new_elements_count = (new_elements_count) ? new_elements_count : 1;

			// update array variables
			array_.count_ = new_elements_count;
			array_.size_  = sizeof(type) * array_.count_;

			// free current memory
			if (array_.begin_) core::dynamic_array<type>::deallocate(array_, true);

			// reallocate new memory
			if (array_.allocator) {
				array_.begin_ = (type*)array_.allocator->allocate(array_.size_);
			}
			else {
				array_.begin_ = (type*)core::global_memory::allocate(array_.size_);
			}

			// update array variables
			array_.end_ = array_.begin_ + array_.count_;
		}

		static inline void reallocate(core::dynamic_array<type>& array_ , bool destruct_elements) {
			u32 old_count = array_.count_;

			core::dynamic_array<type>::deallocate(array_, destruct_elements);
			core::dynamic_array<type>::allocate(array_, old_count);
		}


	}; // class dynamic_array end

} // namespace core end

#endif