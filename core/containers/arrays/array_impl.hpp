#if 1
#pragma once

#ifndef CORE_ARRAY_HPP_IMPL
#define CORE_ARRAY_HPP_IMPL

#include "core/logger/logger.hpp"
#include "core/strings/string.hpp"

#include "array.hpp"

#ifdef DEBUG
	static inline auto _arr_hpp_lgr_ = CORE_GET_LOGGER(DATA_STRUCTER_LOGGER);
#else 
	static inline auto _arr_hpp_lgr_ = nullptr;
#endif

/*
	constructor's
*/
template<typename type>
core::array<type>::array(u32 elements_count, core::dynamic_allocator const& _allocator, memory_tag tag_) NOEXP {

	this->allocate = &_allocator;
	this->_capacity_ = (elements_count) ? elements_count : 1;
	this->_size_ = (this->_capacity_ * sizeof(type));
	this->_tag_ = tag_;

	// allocate memory
	this->_handle_ = this->_allocator_->allocate(
		memory_request{
			.alignement = 0 , .size = this->_size_ , .tag = tag_
		}
	);

	if (this->_handle_.response != allocator_response::success) {
		CORE_FATAL_HPP(_arr_hpp_lgr_, CORE_LOG_CONFIG_ALL, "failed to allocate memory for core::array<{}>", this->_typename_);
		return;
	}

	this->_begin_ = (type*)_handle_.ptr;
	this->_end_   = this->_begin_ + this->_capacity_;

	/*
	// construct elements
	for (type* ptr = this->_begin_; ptr != this->_end_; ptr++) {
		new (ptr) type();
	}
	*/

	CORE_DEBUG_HPP(_arr_hpp_lgr_,0, "new core::array< {} >[{}] created for {} with size {}.", this->_typename_, this->_capacity_, core::to_string(tag_), this->_size_);
}

// copy constructor
template<typename type>
core::array<type>::array(core::array<type> const& array_to_copy, core::dynamic_allocator const& _allocator) NOEXP {
	this->allocate = &_allocator;

	if (this == &array_to_copy) {
		CORE_WARN_HPP(
			_arr_hpp_lgr_,0,
			CORE_SELF_ASSIGN_BUG,
			core::pointer_to_hex_string(&array_to_copy),
			core::pointer_to_hex_string(this)
		);
		return;
	}

	this->_capacity_ = array_to_copy._capacity_ ? array_to_copy._capacity_ : 1;
	this->_size_ = sizeof(type) * this->_capacity_;
	this->_tag_  = array_to_copy._tag_;

	// allocate memory
	this->_handle_ = this->_allocator_->allocate(
		memory_request{
			.alignement = 0 , .size = this->_size_ , ._tag_ = this->_tag_
		}
	);

	if (this->_handle_.response != allocator_response::success) {
		CORE_FATAL_HPP(_arr_hpp_lgr_, CORE_LOG_CONFIG_ALL, "failed to allocate memory for core::array<{}>", this->_typename_);
		return;
	}

	this->_begin_ = (type*)_handle_.ptr;
	this->_end_   = this->_begin_ + this->_capacity_;

	// todo: add multi-thread copying for large data !
	if constexpr (std::is_trivially_copyable<type>::value) {
		// copy elements if copyable 
		std::memcpy(this->_begin_, array_to_copy._begin_, this->_size_);
	}
	else { // or copy construct
		for (u32 i = 0; i < this->_capacity_; i++) {
			new (this->_begin_ + i) type(array_to_copy._begin_[i]);
		}
	}

	CORE_DEBUG_HPP(_arr_hpp_lgr_,0,"new core::array<{}>[{}] created for {} with size {}.", this->_typename_, this->_capacity_, core::to_string(_tag_), this->_size_);
}

// move constructor 
template<typename type>
core::array<type>::array(core::array<type>& array_to_move) NOEXP {

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
	this->_allocator_ = array_to_move._allocator_;
	this->_begin_     = array_to_move._begin_;
	this->_end_       = array_to_move._end_;
	this->_size_      = array_to_move._size_;
	this->_capacity_  = array_to_move._capacity_;
	this->_tag_       = array_to_move._tag_;

	// clear other array
	array_to_move._allocator_ = nullptr;
	array_to_move._begin_    = nullptr;
	array_to_move._end_      = nullptr;
	array_to_move._size_     = 0;
	array_to_move._capacity_    = 0;

	CORE_DEBUG_HPP(_arr_hpp_lgr_, 0, "moved ownership to new core::array< {} >[{}] created for {} with size {} .", this->_typename_, this->_capacity_, core::to_string(_tag_), this->_size_);
}

/*
	destructor
*/
template<typename type>
core::array<type>::~array() NOEXP {

	// destroy all elements in array is destructable
	if (this->_begin_ != nullptr) {

		if constexpr (!std::is_trivially_destructible<type>::value) {
			for (type* ptr = this->_begin_; ptr != this->_end_; ptr++) {
				ptr->~type();
			}
		}

	}

	// deallocate array memory
	if (this->_begin_) {
		this->_allocator_->deallocate(this->_handle_);
	}

	CORE_DEBUG_HPP(_arr_hpp_lgr_,0, "destructed core::array< {} >[{}] used for {} .", this->_typename_, this->_capacity_, core::to_string(_tag_));
}


/*
	operator's
*/

template<typename type>
type& core::array<type>::operator[](u32 const& index) NOEXP {

	if (index < this->_capacity_) return *(this->_begin_ + index);
	
	#ifdef DEBUG
		CORE_ERROR_HPP(_arr_hpp_lgr_, CORE_LOG_CONFIG_ALL , CORE_INDEX_OUT_OF_RANGE, index, "core::array<{}>", this->_typename_);
		CORE_WARN_HPP(_arr_hpp_lgr_,0,CORE_WARNINIG_RUNTIME_CRASH);
	#endif

	return *(this->_begin_ + this->_capacity_); // access out of range value
}

/*
	array public functions
*/

template<typename type>
type* core::array<type>::begin() NOEXP {
	return this->_begin_;
}

template<typename type>
const type* core::array<type>::begin() const NOEXP {
	return this->_begin_;
}

template<typename type>
type* core::array<type>::end() NOEXP {
	return this->_end_;
}

template<typename type>
const type* core::array<type>::end() const NOEXP {
	return this->_end_;
}

template<typename type>
void core::array<type>::clear() NOEXP {
	if (this->_begin_) {
		std::memset(this->_begin_, 0, this->_size_);
	}
}

template<typename type>
u32 core::array<type>::elements_count() NOEXP {
	return this->_capacity_;
}

template<typename type>
u64 core::array<type>::size() NOEXP {
	return this->_size_;
}


template<typename type>
bool core::array<type>::resize() NOEXP {

	type* new_buffer = nullptr;
	u32   new_count  = this->_count_ + this->_resize_value_;
	u32   new_size   = sizeof(type) * new_count;

	// allocate new memory
	memory_handle new_handle = this->_allocator_->allocate(
		memory_request{
			.alignement = 0,
			.size = new_size,
			.tag  = this->_tag_
		}
	);
	// "roll-back if failed to get new memory !
	if (new_handle.response != allocator_response::success) {
		return false;
	}

	new_buffer = (type*)new_handle.ptr;

	// move elements to new memory
	if constexpr (std::is_trivially_copyable<type>::value) {
		std::memmove(new_buffer, this->_begin_, this->size);
	}
	else {
		// copying none trivial elements using 'placement new'
		for (u32 i = 0; i < this->_count_; i++) {
			new (new_buffer + i) type(this->_begin_[i]);
		}
	}

	// deallocate old memory
	this->_allocator_->deallocate(this->_handle_);
	
	// update variables
	this->_handle_ = new_handle;
	this->_count_  = new_count;
	this->_size_   = new_size;
	this->_begin_  = new_buffer;
	this->_end_    = this->_begin_ + this->_count_;

	return true;
}

template<typename type>
void core::array<type>::set_resize_value(u32 elements_count) {
	this->_resize_value_ = elements_count ? elements_count : 1;
}


/*
		array static public functions
*/

template<typename type> 
void core::array<type>::copy_elements(core::array<type> const& source, core::array<type>& destination) NOEXP {

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
void core::array<type>::move_elements(core::array<type>& source, core::array<type>& destination) NOEXP {

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
void core::array<type>::move_ownership(core::array<type>& source, core::array<type>& destination) NOEXP {

	if ((&source) == (&destination)) {
	#ifdef DEBUG
		CORE_WARN_HPP(_arr_hpp_lgr_, 0, CORE_SELF_ASSIGN_BUG, PTR_TO_STRING(&source), PTR_TO_STRING(&destination) );
	#endif

		return;
	}

	if (source._begin_ && destination._begin_) {
		destination._allocator_->deallocate(destination._handle_);

		std::memmove(&destination, &source, sizeof(core::array<type>));
		std::memset(&source, 0, sizeof(core::array<type>));
	}

}

// todo : add option for multi-threaded filling later
template<typename type>
void core::array<type>::fill(core::array<type>& _array_, type const& fill_value) NOEXP {

	if (_array_._begin_ == nullptr) {
	#ifdef DEBUG
		CORE_WARN_HPP(_arr_hpp_lgr_, 0, CORE_NULLPTR_BUG , "core::array::fill");
		DEBUG_BREAK;
	#endif

		return;
	}

	std::fill<type>(_array_._begin_, _array_._end_, fill_value);
}

template<typename type>
INLINE void core::array<type>::sort(
	core::array<type>& _array_, bool (*compare_function)(type const& a, type const& b)
) NOEXP {

	if (_array_._begin_) {
		std::sort<type>(_array_._begin_, _array_._end_ , compare_function);
	}
}

template<typename type>
void core::array<type>::reallocate(core::array<type>& _array_, bool destruct_elements) NOEXP {

	// allocate new memory
	memory_handle new_handle = _array_._allocator_->allocate(
		memory_request{
			.alignement = 0,
			.size = _array_._size_,
			.tag  = _array_._tag_
		}
	);

	// move elements to new memory
	std::memmove(new_handle.ptr, _array_._begin_, _array_._size_);

	// deallocate old memory
	_array_._allocator_->deallocate(_array_._handle_);

	// update array variables
	_array_._handle_ = new_handle;
	_array_._begin_  = (type*)new_handle.ptr;
	_array_._end_    = _array_._begin_ + _array_._capacity_;

}


#endif
#endif