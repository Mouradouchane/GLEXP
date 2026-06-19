#pragma once 

#ifndef CORE_ARRAY_HPP
#define CORE_ARRAY_HPP

#include <algorithm>
#include <cstring>
#include <typeinfo>
#include <type_traits>

#include "core/types.hpp"
#include "core/assert.hpp"
#include "core/memory/dynamic/dynamic_allocator.hpp"


#ifdef DEBUG
	static auto _dssa_logger_ = CORE_GET_LOGGER(DATA_STRUCTER_LOGGER);
#else 
	static auto _dssa_logger_ = nullptr;
#endif

#define _LOGGER_ _dssa_logger_

namespace core {

	/*
		class for fixed count array , array memory allocated on the heap not in stack
	*/
	template<typename type> class array {

	protected:
		core::dynamic_allocator* allocator;
		u32   size_  = 0;
		u32   count_ = 0;
		type* begin_ = nullptr;
		type* end_   = nullptr;

	public:
		// extra variables for personal use :)
		u32 a, b, c, d;

		/*
			constructor's
		*/
		array(u32 elements_count , core::dynamic_allocator const& _allocator)
			:allocator(&_allocator) 
		{
			this->count_ = (elements_count) ? elements_count : 1;
			this->size_  = (this->count_ * sizeof(type));

			// allocate memory
			if (this->allocator == nullptr) {
				this->begin_ = (type*)(core::memory::allocate(this->size_));
			}
			else {
				this->begin_ = (type*)(this->allocator->allocate(this->size_));
			}

			this->end_ = this->begin_ + this->count_;

			// construct elements
			for (type* ptr = this->begin_; ptr != this->end_; ptr++) {
				new (ptr) type();
			}
			
			CORE_DEBUG(0, "new core::array< {} >[{}] created .", type_info(type).name , this->count_);
		}
		
		array(const type* elements, u32 elements_count, core::dynamic_allocator* _allocator = nullptr)
			:allocator(_allocator)
		{

			this->count_ = (elements_count) ? elements_count : 1;
			this->size_  = (this->count_ * sizeof(type));

			if (this->allocator == nullptr) {
				this->begin_ = (type*)core::memory::allocate(this->size_);
			}
			else {
				this->begin_ = (type*)this->allocator->allocate(this->size_);
			}

			this->end_ = this->begin_ + this->count_;

			// todo: multi-threaded copying !
			if constexpr(std::is_trivially_copyable<type>::value) {
				// memcpy elements if copyable 
				std::memcpy(this->begin_, elements, this->size_);
			}
			else {
				for (u32 i = 0; i < this->count_; ++i) {
					new (this->begin_ + i) type(elements[i]);
				}
			}

			CORE_DEBUG(0, "new core::array< {} >[{}] created .", type_info(type).name, this->count_);
		}

		// copy constructor
		array(core::array<type> const& other_array, core::dynamic_allocator* _allocator = nullptr) 
			: allocator(_allocator)
		{
			if (this == &other_array) {
				CORE_WARN_D(
					CORE_SELF_ASSIGN_BUG , 
					core::pointer_to_hex_string(&other_array) , 
					core::pointer_to_hex_string(this)
				);
				return;
			}

			this->count_ = other_array.count_ ? other_array.count_ : 1;
			this->size_  = sizeof(type) * this->count_;

			// allocate array memory 
			if (this->allocator) {
				this->begin_ = (type*)this->allocator->allocate(this->size_);
			}
			else {
				this->begin_ = (type*)core::memory::allocate(this->size_);
			}

			this->end_ = this->begin_ + this->count_;

			// todo: add multi-thread copying for large data !
			if constexpr(std::is_trivially_copyable<type>::value) {
				// copy elements if copyable 
				std::memcpy(this->begin_ , other_array.begin_ , this->size_);
			}
			else { 
				for (u32 i = 0; i < this->count_; i++) {
					new (this->begin_ + i) type(other_array.begin_[i]);
				}
			}

			CORE_DEBUG(0, "new core::array< {} >[{}] created .", type_info(type).name, this->count_);
		}
		
		// move constructor 
		array(core::array<type>&& array_to_move) noexcept {
			if (this == &array_to_move) {
				CORE_WARN_D(
					CORE_SELF_ASSIGN_BUG , 
					core::pointer_to_hex_string(&array_to_move),
					core::pointer_to_hex_string(this)
				);
				return;
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

			CORE_DEBUG(0, "moved ownership to new core::array< {} >[{}] .", type_info(type).name, this->count_);
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
					core::memory::deallocate(this->begin_);
				}

			}

			CORE_DEBUG(0, "core::array< {} >[{}] destructed .", type_info(type).name, this->count_);
		}

		/*
			operator's
		*/ 
		type& operator[](u32 index) {

			if (index >= this->count_) {
				#ifdef UNIT_TEST
					CORE_FATAL_F(
						"core::array<>[ ] -> index {} is out of array range {} , this will cause crash in runtime !",
						index , this->count_
					);
					return *(this->begin_ + this->count_);
				#else 
					CORE_FATAL_F( CORE_INDEX_OUT_OF_RANGE , index, "core::array");
				#endif
			}

			return *(this->begin_ + index); 
		}

		// note: operator=  performe copy operation
		// note: operator=  discard old elements
		core::array<type>& operator = (core::array<type> const& array_to_copy) {
			if (this == &array_to_copy) {
				CORE_WARN_D(
					CORE_SELF_ASSIGN_BUG,
					core::pointer_to_hex_string(&array_to_copy),
					core::pointer_to_hex_string(this)
				);
				return *this;
			}

			core::array<type>::copy(array_to_copy, *this);
			return *this;
		}

		// note: operator=  performe move ownership operation
		// note: operator=  discard old elements
		core::array<type>& operator = (core::array<type>&& array_to_move) {
			if (this == &array_to_move) {
				CORE_WARN_D(
					CORE_SELF_ASSIGN_BUG,
					core::pointer_to_hex_string(&array_to_move),
					core::pointer_to_hex_string(this)
				);
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
					core::memory::deallocate(this->begin_);
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

			CORE_INFO("core::array<{}> -> moved array ownership from {} to {}", typeid(type).name(), 
				PTR_TO_STRING(&array_to_move) , PTR_TO_STRING(this)
			);
			return *this;
		}

		/*
			array public functions
		*/
		type& get(u32 index) {

			if (index >= this->count_) {
				#ifdef UNIT_TEST
					CORE_FATAL_F(
						"core::array<>::get() -> index {} is out of array range {} , this will cause crash in runtime !",
						index , this->count_
					);
					return *(this->begin_ + this->count_);
				#else 
					CORE_FATAL_F( CORE_INDEX_OUT_OF_RANGE , index, "core::array");
				#endif
			}

			return *(this->begin_ + index);
		}

		void set(u32 index, type const& new_element) {
			
			if (index >= this->count_) {
				#ifdef UNIT_TEST
					CORE_FATAL_F(
						"core::array::set() -> index {} is out of array range {} , this will cause crash in runtime !",
						index , this->count_
					);
					return ;
				#else 
					CORE_FATAL_F( CORE_INDEX_OUT_OF_RANGE , index, "core::array");
				#endif
			}
			else *(this->begin_ + index) = new_element;

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

	public:

		/*
				array static public functions
		*/ 

		// note: copying will discard old elements
		// note: there's no checks or safety against overlapped arrays .
		//       why because core::array designed to not have overlapped/shared memory between array .
		//       so any overlapped arrays is probablly a ---> bug in code !
		static void copy(core::array<type> const& source, core::array<type>& destination) {

			if ( (&source) == (&destination) ) {

				#if defined(UNIT_TEST) || defined(DEBUG)
					CORE_WARN_F(
						CORE_SELF_ASSIGN_BUG ", will cause crash in runtime !",
						PTR_TO_STRING(&source) , PTR_TO_STRING(&destination)
					);

					DEBUG_BREAK;
					return;
				#else 
					CORE_FATAL_F(
						CORE_SELF_ASSIGN_BUG , 
						PTR_TO_STRING(&source), PTR_TO_STRING(&destination)
					);
					return;
				#endif

			}
			
			if(source.begin_){
				
				// allocate memory in destination if not allocated yet
				if (destination.begin_ == nullptr) core::array<type>::allocate(destination, source.count_);
				
				// note: source > destination is counted as a bug
				if (source.size_ > destination.size_) {
					#if defined(UNIT_TEST) || defined(DEBUG)
						CORE_WARN_F(
							CORE_SRC_BIGGER_THAN_DEST CORE_WARNING_RUNTIME_CRASH ,
							PTR_TO_STRING(&source), PTR_TO_STRING(&destination)
						);

						DEBUG_BREAK;
						return;
					#else
						CORE_FATAL_F(
							CORE_SRC_BIGGER_THAN_DEST,
							PTR_TO_STRING(&source), PTR_TO_STRING(&destination)
						);
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
			else {
				#if defined(UNIT_TEST) || defined(DEBUG)
					CORE_WARN_F(
						CORE_NULLPTR_BUG CORE_WARNING_RUNTIME_CRASH , "core::array"
					);
					DEBUG_BREAK;
				#else 
					CORE_FATAL_F(CORE_NULLPTR_BUG , "core::array");
				#endif
			}

		}

		static void move(core::array<type>& source, core::array<type>& destination) {
			
			if ( (&source) == (&destination) ) {

				#if defined(UNIT_TEST) || defined(DEBUG)
					CORE_WARN_F(
						CORE_SELF_ASSIGN_BUG CORE_WARNING_RUNTIME_CRASH, 
						PTR_TO_STRING(&source), PTR_TO_STRING(&destination)
					);
					DEBUG_BREAK;
				#else 
					CORE_FATAL_F(
						CORE_SELF_ASSIGN_BUG ,
						PTR_TO_STRING(&source), PTR_TO_STRING(&destination)
					);
				#endif

				return;
			}

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

			if (_array.begin_ == nullptr) {
				#if defined(UNIT_TEST) || defined(DEBUG)
					CORE_WARN_F(CORE_NULLPTR_BUG CORE_WARNING_RUNTIME_CRASH , "core::array");
					DEBUG_BREAK;
				#else 
					CORE_FATAL_F(CORE_NULLPTR_BUG , "core::array");
				#endif
				
				return;
			}

			std::fill(_array.begin_ , _array.end_ , fill_value);
		}

		static inline void sort(
			core::array<type>& _array , bool (*compare_function)(type const& a, type const& b)
		) noexcept {

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
					core::memory::deallocate(_array.begin_);
				}
			}

			_array.begin_  = nullptr;
			_array.end_    = nullptr;
			_array.count_ = 0;
			_array.size_  = 0;
		}

		static inline void allocate(core::array<type>& _array , u32 new_elements_count , bool construct_objects = false) {

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
				_array.begin_ = (type*)core::memory::allocate(_array.size_);
			}

			// construct objects "optional"
			if (construct_objects) {
				for (type* ptr = _array.begin_ ; ptr != _array.end_ ; ptr++) {
					new (ptr) type();
				}
			}

			// update array variables
			_array.end_ = (_array.begin_ + _array.count_);
		}

	}; // class array end

} // namespace core end

#endif