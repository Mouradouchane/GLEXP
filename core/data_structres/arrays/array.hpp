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
		type* begin_ = nullptr;
		type* end_   = nullptr;

	public:
		/*
			constructor's
		*/
		array(u32 elements_count , core::memory_allocator* _allocator = nullptr)
			:allocator(_allocator) 
		{
			this->count_ = (elements_count) ? elements_count : 1;
			this->size_  = (this->count_ * sizeof(type));

			if (this->allocator == nullptr) {
				this->begin_ = (type*)(core::global_memory::allocate(this->size_));
			}
			else {
				this->begin_ = (type*)(this->allocator->allocate(this->size_));
			}

			this->end_ = this->begin_ + this->count_;

			CORE_INFO("core::array<{}> allocated with size:{} at address:&{} ", typeid(type).name(), this->size_ , (void*)this);
		}
		
		array(const type* elements, u32 elements_count, core::memory_allocator* _allocator = nullptr)
			:allocator(_allocator)
		{

			this->count_ = (elements_count) ? elements_count : 1;
			this->size_  = (this->count_ * sizeof(type));

			if (this->allocator == nullptr) {
				this->begin_ = (type*)core::global_memory::allocate(this->size_);
			}
			else {
				this->begin_ = (type*)this->allocator->allocate(this->size_);
			}

			this->end_ = this->begin_ + this->count_;

			// memcpy elements if copyable 
			if constexpr (std::is_trivially_copyable<type>::value) {
				std::memcpy(this->begin_, elements, this->size_);
			}
			else { // manual copying

				// todo: multi-threaded copying
				for (u32 i = 0; i < this->count_; ++i) {
					(this->begin_ + i) = type(elements[i]);
				}
			}

			CORE_INFO("allocated core::array<{}> -> address:&{} , size:{}", typeid(type).name(), (void*)&this , this->size_);
		}

		// copy constructor
		array(core::array<type> const& other_array, core::memory_allocator* _allocator = nullptr) 
			: allocator(_allocator)
		{
			this->count_ = other_array.count_;
			this->size_  = other_array.size_;

			// allocate array memory 
			if (this->allocator == nullptr) {
				this->begin_ = (type*)core::global_memory::allocate(this->size_);
				CORE_WARN("core::array<{}> allocated using core::global_memory allocator .", typeid(type).name());
			}
			else {
				this->begin_ = (type*)this->allocator->allocate(this->size_);
			}

			this->end_ = this->begin_ + this->count_;

			if constexpr (std::is_trivially_copyable<type>::value) {
				std::memcpy(this->begin_ , other_array.begin_ , this->size_);
			}
			else { 
				// todo: multi-threaded copying
				for (u32 i = 0; i < this->count_; i++) {
					this->begin_[i] = other_array.begin_[i];
				}
			}

			CORE_INFO("allocated core::array<{}> -> address:&{} , size:{}", typeid(type).name(), (void*)this , this->size_);
		}

		/*
			destructor
		*/
		~array() {

			// destroy all elements in array is destructable
			if (this->begin_ != nullptr) {
				if constexpr(!std::is_trivially_destructible<type>::value) {

					for (type* ptr = this->begin_; ptr != this->end_; ptr++) {
						ptr->~type();
					}
				}
			}

			// free array memory
			if (this->begin_) {

				if (this->allocator != nullptr) {
					this->allocator->deallocate(this->begin_);
				}
				else {
					core::global_memory::deallocate(this->begin_);
				}

			}

			CORE_INFO("deallocated core::array<{}> at address:&{} with size:{}", typeid(type).name(), (void*)this, this->size_);
		}
		
		/*
			array public functions
		*/
		type& get(u32 index) {
			VCRASH_IF(index >= this->count_ , "error at core::array.get() -> index '{}' out of array range '{}' !" , index , this->count_);

			return *(this->begin_ + index);
		}

		void set(u32 index, type const& new_element) {
			VCRASH_IF(index >= this->count_ , "error at core::array.set() -> index '{}' out of array range '{}' !" , index , this->count_);

			*(this->begin_ + index) = new_element;
		}

		type* begin() noexcept {
			return this->begin_;
		}

		type* end() noexcept {
			return this->end_;
		}

		void clear() {
			if (this->begin_) {

				if constexpr (std::is_trivially_copyable<type>::value) {
					std::memset(this->begin_, 0, this->size_);
				} 
				else {
					for (u32 i = 0; i < count_; i++) {
						this->begin_[i] = type();
					}
				}

			}
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
			VCRASH_IF(index >= this->count_ , "error at core::dynamic_array operator[] : index '{}' out of array range '{}' !" , index , this->count_);
			return *(this->begin_ + index); 
		}

		// note: operator = performe a copy operation
		core::array<type>& operator = (core::array<type>& array_to_copy) {
			if (this == &array_to_copy) return *this;

			core::array<type>::copy(&array_to_copy, this);
			return *this;
		}

		// move operator
		core::array<type>& operator = (core::array<type>&& other) {
			if (this == &other) return *this;

			if (this->begin_ != nullptr) {
				// deallocate current elements
				if constexpr (!std::is_trivially_destructible<type>::value) {
					for (type* p = this->begin_; p != this->end_; ++p) {
						p->~type();
					}
				}

				// deallocate current array
				if (this->allocator) {
					this->allocator->deallocate(this->begin_);
				} else {
					core::global_memory::deallocate(this->begin_);
				}
			}

			// move ownership
			this->allocator = other.allocator;
			this->begin_    = other.begin_;
			this->end_      = other.end_;
			this->size_     = other.size_;
			this->count_    = other.count_;

			// clear source
			other.allocator = nullptr;
			other.begin_    = nullptr;
			other.end_      = nullptr;
			other.size_     = 0;
			other.count_    = 0;

			return *this;
		}

	public:

		/*
				array static public functions
		*/ 

		static void copy(core::array<type> const& source, core::array<type>& destination) {
			VCRASH_IF((&source) == (&destination), "core::array::copy(source={}, destination={}) : copying source to it self is a bug !" , (void*)&source, (void*)&destination);
			CORE_WARN_IF(source.size_ > destination.size_ , "core::array::copy(source={}, destination={}) : source array is bigger than the destination array !", (void*)&source, (void*)&destination);
			
			if (source.begin_) {
				
				// allocate memory in destination if not allocated yet
				if (destination.begin_ == nullptr) core::array<type>::allocate(destination, source.count_);
				
				if constexpr (std::is_trivially_copyable<type>::value) {
					u32 copy_size = (source.size_ > destination.size_) ? destination.size_ : source.size_;
					std::memcpy(destination.begin_, source.begin_, copy_size);
				}
				else {
					u32 copy_count = (source.count_ > destination.count_) ? destination.count_ : source.count_;
					// todo: multi-threaded copying
					for (u32 i = 0; i < copy_count; i++) {
						destination.begin_[i] = source.begin_[i];
					}
				}
			}

		}

		static void move(core::array<type>& source, core::array<type>& destination) {
			VCRASH_IF((&source) == (&destination), "core::array::move(source={}, destination={}) : moving source to it self is a bug !" , (void*)&source, (void*)&destination);

			if (destination.begin_) {
				core::array<type>::deallocate(destination, true);
			}

			// move ownership to destination
			destination.allocator = source.allocator;
			destination.begin_     = source.begin_;
			destination.end_       = source.end_;
			destination.size_     = source.size_;
			destination.count_    = source.count_;

			// clean source
			source.allocator = nullptr;
			source.begin_     = nullptr;
			source.end_       = nullptr;
			source.size_     = 0;
			source.count_    = 0;
		}

		// todo : add option for multi-threaded copying later
		static void fill(array<type>& _array, type const& fill_value) noexcept {
			VCRASH_IF(_array.begin_ == nullptr , "core::array::fill(_array={}) : array memory is null-pointer !" , (void*)&_array);

			std::fill(_array.begin_ , _array.end_ , fill_value);
		}

		static inline void array<type>::sort(
			core::array<type>& _array,
			bool (*compare_function)(type const& a, type const& b)
		) noexcept 
		{
			std::sort(_array.begin_ , _array.end_ , compare_function);
		}

		/*
			few helper functions
		*/ 

		static inline void deallocate(core::array<type>& _array , bool destruct_elements) {

			if (!_array.begin_) {
				_array.begin_  = nullptr;
				_array.end_    = nullptr;
				_array.count_ = 0;
				_array.size_  = 0;

				return;
			}

			// destruct elements if destructable 
			if (destruct_elements) {
				if constexpr (!std::is_trivially_destructible<type>::value) {

					// todo: maybe multi-threaded destruction if possible !!!
					for (type* obj = _array.begin_; obj != (_array.begin_ + _array.count_); obj++) {
						obj->~type();
					}
				}
			}

			// deallocate array memory
			if(_array.begin_){
				if (_array.allocator) {
					_array.allocator->deallocate(_array.begin_);
				}
				else core::global_memory::deallocate(_array.begin_);
			}

			_array.begin_  = nullptr;
			_array.end_    = nullptr;
			_array.count_ = 0;
			_array.size_  = 0;
		}

		static inline void allocate(core::array<type>& _array , u32 new_elements_count) {

			CRASH_IF(new_elements_count == 0 , "core::array::allocate : 0 size allocation is not allowed !");

			_array.count_ = new_elements_count;
			_array.size_  = sizeof(type) * _array.count_;

			// free current memory
			if (_array.begin_) core::array<type>::deallocate(_array, true);

			// reallocate new memory
			if (_array.allocator) {
				_array.begin_ = (type*)_array.allocator->allocate(_array.size_);
			}
			else {
				_array.begin_ = (type*)core::global_memory::allocate(_array.size_);
			}

			// update array variables
			_array.end_ = _array.begin_ + _array.count_;
		}

		static inline void reallocate(core::array<type>& _array , bool destruct_elements) {
			u32 old_count = _array.count_;

			core::array<type>::deallocate(_array, destruct_elements);
			core::array<type>::allocate(_array, old_count);
		}

	}; // class array end

} // namespace core end

#endif