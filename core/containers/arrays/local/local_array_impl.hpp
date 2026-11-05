#if 1
#pragma once

#ifndef CORE_LOCAL_ARRAY_IMPL_HPP
#define CORE_LOCAL_ARRAY_IMPL_HPP

#include <algorithm>
#include <cstring>
#include <type_traits>

#include "core/logger/logger.hpp"
#include "core/strings/string.hpp"

#include "local_array.hpp"

#ifdef DEBUG
	static inline auto _arr_hpp_lgr_ = CORE_GET_LOGGER(DATA_STRUCTER_LOGGER);
#else 
	static inline auto _arr_hpp_lgr_ = nullptr;
#endif

/*
	constructor's
*/
template<typename type>
core::local_array<type>::local_array(u32 elements_count, core::dynamic_allocator const& allocator, memory_tag tag) NOEXP {

	_capacity_ = (elements_count) ? elements_count : 1;
	_size_     = (_capacity_ * sizeof(type));

	// allocate memory
	_handle_ = _allocator_->allocate(
		memory_request{
			.size = _size_ , 
			.alignement = 0 , 
			.tag = tag,
		}
	);

	if (_handle_.response != allocator_response::success) {
		CORE_FATAL_HPP(_arr_hpp_lgr_, CORE_LOG_CONFIG_ALL, "core::local_array : failed to allocate memory !", _typename_);
		return;
	}

	_begin_ = (type*)_handle_.pointer();

	/*
		construct elements
	*/
	for (type* ptr = _begin_; ptr != _end_; ptr++) {
		new (ptr) type();
	}

	CORE_DEBUG_HPP(_arr_hpp_lgr_,0, "new core::local_array< {} >[{}] created for {} with size {}.", _typename_, _capacity_, core::to_string(tag), _size_);
}

// copy constructor
template<typename type>
core::local_array<type>::local_array(core::local_array<type> const& array_to_copy, core::dynamic_allocator const& allocator_) NOEXP {

	if (this == &array_to_copy) {
		CORE_WARN_HPP(
			_arr_hpp_lgr_,0,
			CORE_SELF_ASSIGN_BUG,
			core::pointer_to_hex_string(&array_to_copy),
			core::pointer_to_hex_string(this)
		);
		return;
	}

	// remove old elements
	_allocator_->deallocate(_handle_);

	// copy variables
	_allocator_ = &_allocator;
	_capacity_  = array_to_copy._capacity_ ? array_to_copy._capacity_ : 1;
	_size_ = sizeof(type) * _capacity_;

	// allocate memory
	_handle_ = _allocator_->allocate(
		memory_request{
			.alignement = 0 , .size = _size_ , ._tag_ = array_to_copy._handle_.tag()
		}
	);

	if (_handle_.response != allocator_response::success) {
		CORE_FATAL_HPP(_arr_hpp_lgr_, CORE_LOG_CONFIG_ALL, "failed to allocate memory for core::local_array<{}>", _typename_);
		return;
	}
	
	// get array memory range
	_begin_ = (type*)_handle_.pointer();
	_end_   = _begin_ + _capacity_;

	// todo: add multi-thread copying for large data !
	if constexpr (std::is_trivially_copyable<type>::value) {
		// copy elements if copyable 
		std::memcpy(_begin_, array_to_copy._begin_, _size_);
	}
	else { // or copy construct
		for (u32 i = 0; i < _capacity_; i++) {
			new (_begin_ + i) type(array_to_copy._begin_[i]);
		}
	}

	CORE_DEBUG_HPP(_arr_hpp_lgr_,0,"new core::local_array<{}>[{}] created for {} with size {}.", _typename_, _capacity_, core::to_string(_tag_), _size_);
}

// move constructor 
template<typename type>
core::local_array<type>::local_array(core::local_array<type> && array_to_move) NOEXP {

	if (this == &array_to_move) {
		CORE_WARN_HPP(
			_arr_hpp_lgr_,0,
			CORE_SELF_ASSIGN_BUG,
			core::pointer_to_hex_string(&array_to_move),
			core::pointer_to_hex_string(this)
		);
		return;
	}

	// move ownership
	_allocator_ = array_to_move._allocator_;
	_begin_     = array_to_move._begin_;
	_end_       = array_to_move._end_;
	_size_      = array_to_move._size_;
	_capacity_  = array_to_move._capacity_;
	_tag_       = array_to_move._tag_;

	// clear other array
	array_to_move._allocator_ = nullptr;
	array_to_move._begin_    = nullptr;
	array_to_move._end_      = nullptr;
	array_to_move._size_     = 0;
	array_to_move._capacity_    = 0;

	CORE_DEBUG_HPP(_arr_hpp_lgr_, 0, "moved ownership to new core::local_array< {} >[{}] created for {} with size {} .", _typename_, _capacity_, core::to_string(_tag_), _size_);
}

/*
	destructor
*/
template<typename type>
core::local_array<type>::~array() NOEXP {

	// destroy all elements in array is destructable
	if (_begin_ != nullptr) {

		if constexpr (!std::is_trivially_destructible<type>::value) {
			for (type* ptr = _begin_; ptr != _end_; ptr++) {
				ptr->~type();
			}
		}

	}

	// deallocate array memory
	if (_begin_) {
		_allocator_->deallocate(_handle_);
	}

	CORE_DEBUG_HPP(_arr_hpp_lgr_,0, "destructed core::local_array< {} >[{}] used for {} .", _typename_, _capacity_, core::to_string(_tag_));
}


/*
	operator's
*/

template<typename type>
type& core::local_array<type>::operator[](u32 const& index) NOEXP {

	if (index < _capacity_) return *(_begin_ + index);
	
	#ifdef DEBUG
		CORE_ERROR_HPP(_arr_hpp_lgr_, CORE_LOG_CONFIG_ALL , CORE_INDEX_OUT_OF_RANGE, index, "core::local_array<{}>", _typename_);
		CORE_WARN_HPP(_arr_hpp_lgr_,0,CORE_WARNINIG_RUNTIME_CRASH);
	#endif

	return *(_begin_ + _capacity_); // access out of range value
}

/*
	array public functions
*/

template<typename type>
type* core::local_array<type>::begin() NOEXP {
	return _begin_;
}

template<typename type>
const type* core::local_array<type>::begin() const NOEXP {
	return _begin_;
}

template<typename type>
type* core::local_array<type>::end() NOEXP {
	return _end_;
}

template<typename type>
const type* core::local_array<type>::end() const NOEXP {
	return _end_;
}

template<typename type>
void core::local_array<type>::clear() NOEXP {
	if (_begin_) {
		std::memset(_begin_, 0, _size_);
	}
}

template<typename type>
u32 core::local_array<type>::elements_count() NOEXP {
	return _capacity_;
}

template<typename type>
u64 core::local_array<type>::size() NOEXP {
	return _size_;
}


template<typename type>
bool core::local_array<type>::resize() NOEXP {

	type* new_buffer = nullptr;
	u32   new_count  = _count_ + _resize_value_;
	u32   new_size   = sizeof(type) * new_count;

	// allocate new memory
	memory_handle new_handle = _allocator_->allocate(
		memory_request{
			.alignement = 0,
			.size = new_size,
			.tag  = _tag_
		}
	);
	// "roll-back if failed to get new memory !
	if (new_handle.response != allocator_response::success) {
		return false;
	}

	new_buffer = (type*)new_handle.pointer();

	// move elements to new memory
	if constexpr (std::is_trivially_copyable<type>::value) {
		std::memmove(new_buffer, _begin_, size);
	}
	else {
		// copying none trivial elements using 'placement new'
		for (u32 i = 0; i < _count_; i++) {
			new (new_buffer + i) type(_begin_[i]);
		}
	}

	// deallocate old memory
	_allocator_->deallocate(_handle_);
	
	// update variables
	_handle_ = new_handle;
	_count_  = new_count;
	_size_   = new_size;
	_begin_  = new_buffer;
	_end_    = _begin_ + _count_;

	return true;
}

template<typename type>
void core::local_array<type>::set_resize_value(u32 elements_count) {
	_resize_value_ = elements_count ? elements_count : 1;
}


/*
		array static public functions
*/

template<typename type> 
void core::local_array<type>::copy_elements(core::local_array<type> const& source, core::local_array<type>& destination) NOEXP {

	if ((&source) == (&destination)) {
	#ifdef DEBUG
		CORE_WARN_HPP(_arr_hpp_lgr_, 0, CORE_SELF_ASSIGN_BUG, PTR_TO_STRING(&source), PTR_TO_STRING(&destination));
		CORE_WARN_HPP(_arr_hpp_lgr_, 0, CORE_WARNINIG_RUNTIME_CRASH);

		DEBUG_BREAK;
	#endif

		return;
	}

	if (source._begin_ && destination._begin_) {

		// note: source > destination will get handled as a bug
		if (source._size_ > destination._size_) {
		#ifdef DEBUG
			CORE_ERROR_HPP(
				_arr_hpp_lgr_, CORE_LOG_CONFIG_ALL,
				CORE_SRC_BIGGER_THAN_DEST, PTR_TO_STRING(&source), PTR_TO_STRING(&destination)
			);
			CORE_WARN_HPP(_arr_hpp_lgr_, 0, CORE_WARNINIG_RUNTIME_CRASH);

			DEBUG_BREAK;
		#else
			CORE_FATAL_HPP(
				_arr_hpp_lgr_, 0, CORE_SRC_BIGGER_THAN_DEST,
				PTR_TO_STRING(&source), PTR_TO_STRING(&destination)
			);
		#endif

			return;
		}

		// todo: multi-threaded copying
		if constexpr (std::is_trivially_copyable<type>::value) {
			std::memcpy(destination._begin_, source._begin_, source._size_);
		}
		else {
			for (u32 i = 0; i < source._capacity_; i++) {
				new (destination._begin_ + i) type(source._begin_[i]);
			}
		}

	}
	else {
	#ifdef DEBUG
		CORE_ERROR_HPP(
			_arr_hpp_lgr_, 0,
			CORE_NULLPTR_BUG, "core::array::copy_elements"
		);

		CORE_WARN_HPP(_arr_hpp_lgr_, 0, CORE_WARNINIG_RUNTIME_CRASH);
		DEBUG_BREAK;
	#else 
		CORE_FATAL_F(CORE_NULLPTR_BUG, "core::array::copy_elements");
	#endif
	}

}

// todo : add option for multi-threaded moving elements later
template<typename type>
void core::local_array<type>::move_elements(core::local_array<type>& source, core::local_array<type>& destination) NOEXP {

	if ((&source) == (&destination)) {
	#ifdef DEBUG
		CORE_WARN_HPP(_arr_hpp_lgr_, 0, CORE_SELF_ASSIGN_BUG, PTR_TO_STRING(&source), PTR_TO_STRING(&destination));
		CORE_WARN_HPP(_arr_hpp_lgr_, 0, CORE_WARNINIG_RUNTIME_CRASH);

		DEBUG_BREAK;
	#endif

		return;
	}


	if (source._begin_ && destination._begin_) {

		// note: source > destination will get handled as a bug
		if (source._size_ > destination._size_) {
		#ifdef DEBUG
			CORE_ERROR_HPP(
				_arr_hpp_lgr_, CORE_LOG_CONFIG_ALL,
				CORE_SRC_BIGGER_THAN_DEST, PTR_TO_STRING(&source), PTR_TO_STRING(&destination)
			);
			CORE_WARN_HPP(_arr_hpp_lgr_, 0, CORE_WARNINIG_RUNTIME_CRASH);

			DEBUG_BREAK;
		#else
			CORE_FATAL_HPP(
				_arr_hpp_lgr_, 0, CORE_SRC_BIGGER_THAN_DEST,
				PTR_TO_STRING(&source), PTR_TO_STRING(&destination)
			);
		#endif

			return;
		}

		// todo: multi-threaded moving
		if constexpr (std::is_trivially_copyable<type>::value) {
			std::memmove(destination._begin_, source._begin_, source._size_);
		}
		else {
			for (u32 i = 0; i < source._capacity_; i++) {
				new (destination._begin_ + i) type(source._begin_[i]);
			}
			std::memset(source._begin_, 0, source._size_);
		}

	}
	else {
	#ifdef DEBUG
		CORE_ERROR_HPP(
			_arr_hpp_lgr_, 0,
			CORE_NULLPTR_BUG, "core::array::move_elements"
		);

		CORE_WARN_HPP(_arr_hpp_lgr_, 0, CORE_WARNINIG_RUNTIME_CRASH);
		DEBUG_BREAK;
	#else 
		CORE_FATAL_F(CORE_NULLPTR_BUG, "core::array::move_elements");
	#endif
	}

}

template<typename type>
void core::local_array<type>::move_ownership(core::local_array<type>& source, core::local_array<type>& destination) NOEXP {

	if ((&source) == (&destination)) {
	#ifdef DEBUG
		CORE_WARN_HPP(_arr_hpp_lgr_, 0, CORE_SELF_ASSIGN_BUG, PTR_TO_STRING(&source), PTR_TO_STRING(&destination) );
	#endif

		return;
	}

	if (source._begin_ && destination._begin_) {
		destination._allocator_->deallocate(destination._handle_);

		std::memmove(&destination, &source, sizeof(core::local_array<type>));
		std::memset(&source, 0, sizeof(core::local_array<type>));
	}

}

// todo : add option for multi-threaded filling later
template<typename type>
void core::local_array<type>::fill(core::local_array<type>& _array_, type const& fill_value) NOEXP {

	if (_array_._begin_ == nullptr) {
	#ifdef DEBUG
		CORE_WARN_HPP(_arr_hpp_lgr_, 0, CORE_NULLPTR_BUG , "core::array::fill");
		DEBUG_BREAK;
	#endif

		return;
	}

	std::fill(_array_._begin_, _array_._end_, fill_value);
}

template<typename type>
INLINE void core::local_array<type>::sort(
	core::local_array<type>& _array_, bool (*compare_function)(type const& a, type const& b)
) NOEXP {

	if (_array_._begin_) {
		std::sort(_array_._begin_, _array_._end_ , compare_function);
	}
}

template<typename type>
void core::local_array<type>::reallocate(core::local_array<type>& _array_, bool destruct_elements) NOEXP {

	// allocate new memory
	memory_handle new_handle = _array_._allocator_->allocate(
		memory_request{
			.size = _array_._size_,
			.alignement = 0,
			.tag  = _array_._tag_
		}
	);

	// move elements to new memory
	std::memmove(new_handle.pointer(), _array_._begin_, _array_._size_);

	// deallocate old memory
	_array_._allocator_->deallocate(_array_._handle_);

	// update array variables
	_array_._handle_ = new_handle;
	_array_._begin_  = (type*)new_handle.pointer();
	_array_._end_    = _array_._begin_ + _array_._capacity_;

}


#endif
#endif