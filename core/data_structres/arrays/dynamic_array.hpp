#pragma once 

#ifndef CORE_DYNAMIC_ARRAY_HPP
#define CORE_DYNAMIC_ARRAY_HPP

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

				// deallocate old memory
				if (this->begin_) {
					core::dynamic_array<type>::deallocate(*this, true);
				}

				// pass allocator if it's there
				if (!this->allocator) {
					this->allocator = array_to_copy.allocator;	 
				}

				// allocate new memory
				core::dynamic_array<type>::allocate(*this, array_to_copy.count_);
				
				// update variables
				this->push_index   = array_to_copy.push_index;
				this->resize_value = array_to_copy.resize_value;
				this->count_ = array_to_copy.count_;
				this->size_  = sizeof(type) * this->count_;
				this->end_   = this->begin_ + this->count_;
				
				// copying elements
				if constexpr(std::is_trivially_copyable<type>::value) {
					u32 copy_size = (array_to_copy.size_ > this->size_) ? this->size_ : array_to_copy.size_;
					std::memcpy(this->begin_, array_to_copy.begin_, copy_size);
				}
				else {
					// todo: multi-threaded copying
					u32 copy_count = (array_to_copy.count_ > this->count_) ? this->count_ : array_to_copy.count_;
					for (u32 i = 0; i < copy_count; i++) {
						new (this->begin_ + i) type(array_to_copy.begin_[i]);
					}
				}

			}

			return *this;
		}

		// note: performe move ownership operation
		core::dynamic_array<type>& operator = (core::dynamic_array<type>&& array_to_move) {
			if (this == &array_to_move) return *this;

			if (this->begin_) {

				// destruct current elements
				if constexpr(!std::is_trivially_destructible<type>::value) {
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
			this->allocator    = array_to_move.allocator;
			this->begin_       = array_to_move.begin_;
			this->end_         = array_to_move.end_;
			this->size_        = array_to_move.size_;
			this->count_       = array_to_move.count_;
			this->push_index   = array_to_move.push_index;
			this->resize_value = array_to_move.resize_value;

			// clear other array 
			array_to_move.allocator    = nullptr;
			array_to_move.begin_       = nullptr;
			array_to_move.end_         = nullptr;
			array_to_move.size_        = 0;
			array_to_move.count_       = 0;
			array_to_move.push_index   = 0;
			array_to_move.resize_value = 0;
			
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

		u32 get_push_index() {
			return this->push_index;
		}

		void resize(bool destruct_elements) {
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
				// copying none trivial elements using 'placement new'
				for (u32 i = 0; i < old_count; i++) {
					new (new_buffer + i) type(this->begin_[i]);
				}
			}

			// destruct elements in old memory if needed
			if( destruct_elements ) {

				if constexpr(!std::is_trivially_destructible<type>::value) {
					for (u32 i = 0; i < old_count; i++) {
						this->begin_[i].~type();
					}
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
			if(this->push_index >= this->count_) this->resize(false);
			
			// push then update counter
			new (this->begin_ + this->push_index) type(new_element);
			this->push_index += 1;
		}
		
		/*
			note: using operator [] and push/pop function's in the same time could cause conflict's and overlap's !
			avoid using both of them at once to avoid conflict's and run-time bugs !
		*/
		type pop(bool call_destructor) {
			CRASH_IF(!this->begin_ , "core::dynamic_array::pop() -> array memory is null !");
			
			if (this->push_index) {

				this->push_index -= 1;
				type copy = this->begin_[push_index];

				// call element destructor if needed
				if constexpr (!std::is_trivially_destructible<type>::value) {
					if (call_destructor) {
						this->begin_[push_index].~type();
					}
				}

				return copy;
			}
			else {
				if constexpr(std::is_default_constructible<type>::value) {
					return type();
				}
				else {
					CRASH_IF(true, "core::dynamic_array::pop() -> unable to pop element from empty array !");
				}
			}
	
		}
	
	private:
		/*
			few private helper functions
		*/ 

		static inline void deallocate(core::dynamic_array<type>& array_, bool destruct_elements) {

			if (!array_.begin_) {
				array_.begin_     = nullptr;
				array_.end_       = nullptr;
				array_.count_     = 0;
				array_.size_      = 0;
				array_.push_index = 0;
				
				return;
			}

			// destruct elements if destructable 
			if (destruct_elements) {
				if constexpr(!std::is_trivially_destructible<type>::value) {
					// todo: maybe multi-threaded destruction if possible !!!
					for (type* obj = array_.begin_; obj != (array_.begin_ + array_.count_); obj++) {
						obj->~type();
					}
				}
			}

			// deallocate array memory
			if (array_.allocator) {
				array_.allocator->deallocate(array_.begin_);
			}
			else {
				core::global_memory::deallocate(array_.begin_);
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