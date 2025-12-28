#pragma once 

#ifndef CORE_ARRAY_HPP
#define CORE_ARRAY_HPP

#include <algorithm>
#include <cstring>
#include <typeinfo>
#include <type_traits>

#include "core/types.hpp"
#include "core/assert.hpp"
#include "core/status/status.hpp"
#include "core/memory/memory.hpp"

namespace status = core::status;

#ifdef DEBUG

	#define INFO_ARRAY_CONSTUCTED() CORE_DEBUG( \
		"0x{} -> core::array<{}>[{}] -> allocated using allocator \"{}\" for {} system", \
		(void*)this , typeid(type).name(), this->size_ , \
		(this->allocator ? this->allocator.name() : "global allocator"), \
		(this->allocator ? this->allocator.type() : "unkown !"), \
	);

#else 

	#define INFO_ARRAY_CONSTUCTED()

#endif

namespace core {

	/*
		class for fixed count array , array memory allocated on the heap not in stack
	*/
	template<typename type> class array {

	protected:
		core::memory_allocator* allocator;
		u32   size_  = 0;
		u32   count_ = 0;
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

			// allocate memory
			if (this->allocator == nullptr) {
				this->begin_ = (type*)(core::global_memory::allocate(this->size_));
			}
			else {
				this->begin_ = (type*)(this->allocator->allocate(this->size_));
			}

			this->end_ = this->begin_ + this->count_;

			// construct elements
			for (type* ptr = this->begin_; ptr != this->end_; ptr++) {
				new (ptr) type();
			}

			INFO_ARRAY_CONSTUCTED();
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
			if constexpr(std::is_trivially_copyable<type>::value) {
				std::memcpy(this->begin_, elements, this->size_);
			}
			else {
				// todo: multi-threaded copying
				for (u32 i = 0; i < this->count_; ++i) {
					new (this->begin_ + i) type(elements[i]);
				}
			}

			INFO_ARRAY_CONSTUCTED();
		}

		// copy constructor
		array(core::array<type> const& other_array, core::memory_allocator* _allocator = nullptr) 
			: allocator(_allocator)
		{
			this->count_ = other_array.count_ ? other_array.count_ : 1;
			this->size_  = sizeof(type) * this->count_;

			// allocate array memory 
			if (this->allocator) {
				this->begin_ = (type*)this->allocator->allocate(this->size_);
			}
			else {
				this->begin_ = (type*)core::global_memory::allocate(this->size_);
			}

			this->end_ = this->begin_ + this->count_;

			// todo: multi-threaded copying
			if constexpr(std::is_trivially_copyable<type>::value) {
				std::memcpy(this->begin_ , other_array.begin_ , this->size_);
			}
			else { 
				for (u32 i = 0; i < this->count_; i++) {
					new (this->begin_ + i) type(other_array.begin_[i]);
				}
			}

			INFO_ARRAY_CONSTUCTED();
		}
		
		// move constructor 
		array(core::array<type>&& array_to_move) noexcept {
			if (this == &array_to_move) return;

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

			CORE_INFO("core::array<{}> -> moved array ownership from 0x{} to 0x{}", typeid(type).name(), (void*)&array_to_move , (void*)this);
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

			CORE_DEBUG("core::array<{}>[{}] -> destructed + memory deallocated", typeid(type).name(), this->size_);
		}

		/*
			operator's
		*/ 
		type& operator[](u32 index) {
			VCRASH_IF(index >= this->count_ , core::status::get_error(core::error::index_out_range) , index, this->count_);
			return *(this->begin_ + index); 
		}

		// note: operator=  performe copy operation
		// note: operator=  discard old elements
		core::array<type>& operator = (core::array<type> const& array_to_copy) {
			if (this == &array_to_copy) {
				CORE_WARN( core::status::get_warning(core::warning::self_assignment) );
				return *this;
			}

			core::array<type>::copy(array_to_copy, *this);
			return *this;
		}

		// note: operator=  performe move ownership operation
		// note: operator=  discard old elements
		core::array<type>& operator = (core::array<type>&& array_to_move) {
			if (this == &array_to_move) {
				CORE_WARN(core::status::get_warning(core::warning::self_assignment));
				return *this;
			}

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

			CORE_INFO("core::array<{}> -> moved array ownership from 0x{} to 0x{}", typeid(type).name(), (void*)&array_to_move , (void*)this);
			return *this;
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
		const type* begin() const noexcept {
			return this->begin_;
		}

		type* end() noexcept {
			return this->end_;
		}
		const type* end() const noexcept {
			return this->end_;
		}

		void clear() noexcept {
			if (this->begin_) {

				if constexpr (std::is_trivially_copyable<type>::value) {
					std::memset(this->begin_, 0, this->size_);
				} 
				else {
					for (u32 i = 0; i < this->count_; i++) {
						this->begin_[i] = type();
					}
				}

			}
		}

		u32 count() noexcept {
			return this->count_;
		}

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

	public:

		/*
				array static public functions
		*/ 

		// note: copying will discard old elements
		// note: there's no checks or safety against overlapped arrays .
		//       why because core::array designed to not have overlapped/shared memory between array .
		//       so any overlapped arrays is probablly a ---> bug in code !
		static void copy(core::array<type> const& source, core::array<type>& destination) {
			VCRASH_IF((&source) == (&destination), "core::array::copy(source={}, destination={}) : copying source to it self is a bug !" , (void*)&source, (void*)&destination);
			
			if(source.begin_){
				
				// allocate memory in destination if not allocated yet
				if (destination.begin_ == nullptr) core::array<type>::allocate(destination, source.count_);
				
				// note: source > destination is counted as a bug
				if (source.size_ > destination.size_) {
				#ifdef UNIT_TEST
					CORE_WARN("core::array::copy() : source is bigger than destination , this will cause crash in runtime !");
					return;
				#else
					VCRASH_IF(true , "core::array::copy(source={}, destination={}) : source array is bigger than the destination array !", (void*)&source, (void*)&destination);
				#endif
				}
				
				if constexpr (std::is_trivially_copyable<type>::value) {
					u64 copy_size = (source.size_ > destination.size_) ? destination.size_ : source.size_;
					std::memcpy(destination.begin_, source.begin_, copy_size);
				}
				else {
					u32 copy_count = (source.count_ > destination.count_) ? destination.count_ : source.count_;
					// todo: multi-threaded copying
					for (u32 i = 0; i < copy_count; i++) {
						new (destination.begin_ + i) type(source.begin_[i]);
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
			destination.begin_    = source.begin_;
			destination.end_      = source.end_;
			destination.size_     = source.size_;
			destination.count_    = source.count_;

			// clean source
			source.allocator = nullptr;
			source.begin_    = nullptr;
			source.end_      = nullptr;
			source.size_     = 0;
			source.count_    = 0;
		}

		// todo : add option for multi-threaded copying later
		static void fill(core::array<type>& _array, type const& fill_value) noexcept {
			VCRASH_IF(_array.begin_ == nullptr , "core::array::fill(_array={}) : array memory is null-pointer !" , (void*)&_array);

			std::fill(_array.begin_ , _array.end_ , fill_value);
		}

		static inline void sort(
			core::array<type>& _array,
			bool (*compare_function)(type const& a, type const& b)
		) noexcept 
		{
			if (_array.begin_ && _array.count_ > 1) {
				std::sort(_array.begin_ , (_array.begin_ + _array.count_) , compare_function);
			}
		}

		static void reallocate(core::array<type>& _array , bool destruct_elements) {
			u32 old_count = _array.count_;

			core::array<type>::deallocate(_array, destruct_elements);
			core::array<type>::allocate(_array, old_count);
		}

	private:
		/*
			few private helper functions
		*/ 

		static inline void deallocate(core::array<type>& _array , bool destruct_elements) {

			if (!_array.begin_) {
				_array.begin_  = nullptr;
				_array.end_    = nullptr;
				_array.count_  = 0;
				_array.size_   = 0;

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
				else {
					core::global_memory::deallocate(_array.begin_);
				}
			}

			_array.begin_  = nullptr;
			_array.end_    = nullptr;
			_array.count_ = 0;
			_array.size_  = 0;
		}

		static inline void allocate(core::array<type>& _array , u32 new_elements_count) {

			new_elements_count = (new_elements_count) ? new_elements_count : 1;

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

			// construct objects 
			
			// update array variables
			_array.end_ = _array.begin_ + _array.count_;
		}

	}; // class array end

} // namespace core end

#endif