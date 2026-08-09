#if 1
#pragma once

#ifndef CORE_ARRAY_HPP_IMPL
#define CORE_ARRAY_HPP_IMPL

#include "core/logger/logger.hpp"
#include "array.hpp"

#ifdef DEBUG
	static auto _dssa_logger_ = CORE_GET_LOGGER(DATA_STRUCTER_LOGGER);
#else 
	static auto _dssa_logger_ = nullptr;
#endif

#define _LOGGER_ _dssa_logger_

/*
	constructor's
*/
template<typename type>
core::array<type>::array(u32 elements_count, core::dynamic_allocator const& _allocator, memory_tag tag_) NOEXP {

	this->allocate = &_allocator;
	this->_tag_ = tag_;
	this->_count_ = (elements_count) ? elements_count : 1;
	this->_size_ = (this->_count_ * sizeof(type));

	// allocate memory
	this->_handle_ = this->_allocator_->allocate(
		memory_request{
			.alignement = 0 , .size = this->_size_ , ._tag_ = this->_tag_
		}
	);

	if (this->_handle_.response != allocator_response::success) return;

	this->_begin_ = (type*)_handle_.ptr;
	this->_end_ = this->_begin_ + this->_count_;

	// construct elements
	for (type* ptr = this->_begin_; ptr != this->_end_; ptr++) {
		new (ptr) type();
	}

	CORE_DEBUG(0, "new core::array< {} >[{}] created for {} with size {} .", type_info(type).name, this->_count_, core::to_string(_tag_), this->_size_);
}

// copy constructor
template<typename type>
core::array<type>::array(core::array<type> const& other_array, core::dynamic_allocator const& _allocator) NOEXP {
	this->allocate = &_allocator;

	if (this == &other_array) {
		CORE_WARN_D(
			CORE_SELF_ASSIGN_BUG,
			core::pointer_to_hex_string(&other_array),
			core::pointer_to_hex_string(this)
		);
		return;
	}

	this->_count_ = other_array._count_ ? other_array._count_ : 1;
	this->_size_ = sizeof(type) * this->_count_;

	// allocate memory
	this->_handle_ = this->_allocator_->allocate(
		memory_request{
			.alignement = 0 , .size = this->_size_ , ._tag_ = this->_tag_
		}
	);

	if (this->_handle_.response != allocator_response::success) return;

	this->_begin_ = (type*)_handle_.ptr;
	this->_end_ = this->_begin_ + this->_count_;

	// todo: add multi-thread copying for large data !
	if constexpr (std::is_trivially_copyable<type>::value) {
		// copy elements if copyable 
		std::memcpy(this->_begin_, other_array._begin_, this->_size_);
	}
	else {
		for (u32 i = 0; i < this->_count_; i++) {
			new (this->_begin_ + i) type(other_array._begin_[i]);
		}
	}

	CORE_DEBUG(0, "new core::array< {} >[{}] created for {} with size {} .", type_info(type).name, this->_count_, core::to_string(_tag_), this->_size_);
}

// move constructor 
template<typename type>
core::array<type>::array(core::array<type>&& array_to_move) NOEXP {
	if (this == &array_to_move) {
		CORE_WARN_D(
			CORE_SELF_ASSIGN_BUG,
			core::pointer_to_hex_string(&array_to_move),
			core::pointer_to_hex_string(this)
		);
		return;
	}

	// move ownership
	this->_allocator_ = array_to_move._allocator_;
	this->_begin_    = array_to_move._begin_;
	this->_end_      = array_to_move._end_;
	this->_size_     = array_to_move._size_;
	this->_count_    = array_to_move._count_;

	// clear other array
	array_to_move._allocator_ = nullptr;
	array_to_move._begin_    = nullptr;
	array_to_move._end_      = nullptr;
	array_to_move._size_     = 0;
	array_to_move._count_    = 0;

	CORE_DEBUG(0, "moved ownership to new core::array< {} >[{}] created for {} with size {} .", type_info(type).name, this->_count_, core::to_string(_tag_), this->_size_);
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

	CORE_DEBUG(0, "destructed core::array< {} >[{}] used for {} .", type_info(type).name, this->_count_, core::to_string(_tag_));
}


/*
	operator's
*/

template<typename type>
type& core::array<type>::operator[](u32 index) NOEXP {

	if (index >= this->_count_) {
#ifdef UNIT_TEST
		CORE_FATAL_F(
			"core::array<>[ ] -> index {} is out of array range {} , this will cause crash in runtime !",
			index, this->_count_
		);
		return *(this->_begin_ + this->_count_);
#else 
		CORE_FATAL_F(CORE_INDEX_OUT_OF_RANGE, index, "core::array");
#endif
	}

	return *(this->_begin_ + index);
}

// note: operator=  performe copy operation
// note: operator=  discard old elements
template<typename type>
core::array<type> core::array<type>::operator= (core::array<type> const& array_to_copy) NOEXP
{
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
template<typename type>
core::array<type>& core::array<type>::operator = (core::array<type>&& array_to_move) {
	if (this == &array_to_move) {
		CORE_WARN_D(
			CORE_SELF_ASSIGN_BUG,
			core::pointer_to_hex_string(&array_to_move),
			core::pointer_to_hex_string(this)
		);
		return *this;
	}

	if (this->_begin_ != nullptr) {

		// destruct current elements
		if constexpr (!std::is_trivially_destructible<type>::value) {
			for (type* p = this->_begin_; p != this->_end_; ++p) {
				p->~type();
			}
		}

		// deallocate current array memory
		this->_allocator_->deallocate(this->_handle_);
	}

	// move ownership
	this->_allocator_ = array_to_move._allocator_;
	this->_begin_ = array_to_move._begin_;
	this->_end_ = array_to_move._end_;
	this->_size_ = array_to_move._size_;
	this->_count_ = array_to_move._count_;

	// clear other array 
	array_to_move._allocator_ = nullptr;
	array_to_move._begin_ = nullptr;
	array_to_move._end_ = nullptr;
	array_to_move._size_ = 0;
	array_to_move._count_ = 0;

	CORE_DEBUG(
		"core::array<{}> -> moved array ownership from {} to {}",
		typeid(type).name(), core::pointer_to_hex_string(&array_to_move), core::pointer_to_hex_string(this)
	);

	return *this;
}


/*
	array public functions
*/

template<typename type>
type& core::array<type>::get(u32 index) {

	if (index >= this->_count_) {
#ifdef UNIT_TEST
		CORE_FATAL_F(
			"core::array<>::get() -> index {} is out of array range {} , this will cause crash in runtime !",
			index, this->_count_
		);
		return *(this->_begin_ + this->_count_);
#else 
		CORE_FATAL_F(CORE_INDEX_OUT_OF_RANGE, index, "core::array");
#endif
	}

	return *(this->_begin_ + index);
}

template<typename type>
void core::array<type>::set(u32 index, type const& new_element) {

	if (index >= this->_count_) {
#ifdef UNIT_TEST
		CORE_FATAL_F(
			"core::array::set() -> index {} is out of array range {} , this will cause crash in runtime !",
			index, this->_count_
		);
		return;
#else 
		CORE_FATAL_F(CORE_INDEX_OUT_OF_RANGE, index, "core::array");
#endif
	}
	else *(this->_begin_ + index) = new_element;

}

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

		if constexpr (std::is_trivially_copyable<type>::value) {
			std::memset(this->_begin_, 0, this->_size_);
		}
		else {
			for (u32 i = 0; i < this->_count_; i++) {
				this->_begin_[i] = type();
			}
		}

	}
}

template<typename type>
u32 core::array<type>::count() NOEXP {
	return this->_count_;
}

template<typename type>
u32 core::array<type>::size() NOEXP {
	return this->_size_;
}


/*
		array static public functions
*/

// note: copying will discard old elements
// note: there's no checks or safety against overlapped arrays .
//       why because core::array designed to not have overlapped/shared memory between array .
//       so any overlapped arrays is probablly a ---> bug in code !
template<typename type>
void core::array<type>::copy(core::array<type> const& source, core::array<type>& destination) {

	if ((&source) == (&destination)) {

#if defined(UNIT_TEST) || defined(DEBUG)
		CORE_WARN_F(
			CORE_SELF_ASSIGN_BUG ", will cause crash in runtime !",
			PTR_TO_STRING(&source), PTR_TO_STRING(&destination)
		);

		DEBUG_BREAK;
		return;
#else 
		CORE_FATAL_F(
			CORE_SELF_ASSIGN_BUG,
			PTR_TO_STRING(&source), PTR_TO_STRING(&destination)
		);
		return;
#endif

	}

	if (source._begin_) {

		// allocate memory in destination if not allocated yet
		if (destination._begin_ == nullptr) core::array<type>::allocate(destination, source._count_);

		// note: source > destination is counted as a bug
		if (source._size_ > destination._size_) {
#if defined(UNIT_TEST) || defined(DEBUG)
			CORE_WARN_F(
				CORE_SRC_BIGGER_THAN_DEST CORE_WARNING_RUNTIME_CRASH,
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
			u64 copy_size = (source._size_ > destination._size_) ? destination._size_ : source._size_;
			std::memcpy(destination._begin_, source._begin_, copy_size);
		}
		else {
			u32 copy_count = (source._count_ > destination._count_) ? destination._count_ : source._count_;
			// todo: multi-threaded copying
			for (u32 i = 0; i < copy_count; i++) {
				new (destination._begin_ + i) type(source._begin_[i]);
			}
		}
	}
	else {
#if defined(UNIT_TEST) || defined(DEBUG)
		CORE_WARN_F(
			CORE_NULLPTR_BUG CORE_WARNING_RUNTIME_CRASH, "core::array"
		);
		DEBUG_BREAK;
#else 
		CORE_FATAL_F(CORE_NULLPTR_BUG, "core::array");
#endif
	}

}

template<typename type>
void core::array<type>::move(core::array<type>& source, core::array<type>& destination) {

	if ((&source) == (&destination)) {

#if defined(UNIT_TEST) || defined(DEBUG)
		CORE_WARN_F(
			CORE_SELF_ASSIGN_BUG CORE_WARNING_RUNTIME_CRASH,
			PTR_TO_STRING(&source), PTR_TO_STRING(&destination)
		);
		DEBUG_BREAK;
#else 
		CORE_FATAL_F(
			CORE_SELF_ASSIGN_BUG,
			PTR_TO_STRING(&source), PTR_TO_STRING(&destination)
		);
#endif

		return;
	}

	if (destination._begin_) {
		core::array<type>::deallocate(destination, true);
	}

	// move ownership to destination
	destination._allocator_ = source._allocator_;
	destination._begin_ = source._begin_;
	destination._end_ = source._end_;
	destination._size_ = source._size_;
	destination._count_ = source._count_;

	// clean source
	source._allocator_ = nullptr;
	source._begin_ = nullptr;
	source._end_ = nullptr;
	source._size_ = 0;
	source._count_ = 0;
}

// todo : add option for multi-threaded copying later
template<typename type>
void core::array<type>::fill(core::array<type>& _array, type const& fill_value) NOEXP {

	if (_array._begin_ == nullptr) {
#if defined(UNIT_TEST) || defined(DEBUG)
		CORE_WARN_F(CORE_NULLPTR_BUG CORE_WARNING_RUNTIME_CRASH, "core::array");
		DEBUG_BREAK;
#else 
		CORE_FATAL_F(CORE_NULLPTR_BUG, "core::array");
#endif

		return;
	}

	std::fill(_array._begin_, _array._end_, fill_value);
}

template<typename type>
INLINE void core::array<type>::sort(
	core::array<type>& _array, bool (*compare_function)(type const& a, type const& b)
) NOEXP {

	if (_array._begin_ && _array._count_ > 1) {
		std::sort(_array._begin_, (_array._begin_ + _array._count_), compare_function);
	}
}

template<typename type>
void core::array<type>::reallocate(core::array<type>& _array, bool destruct_elements) {
	u32 old_count = _array._count_;

	core::array<type>::deallocate(_array, destruct_elements);
	core::array<type>::allocate(_array, old_count);
}


/*
	few private helper functions
*/


template<typename type>
INLINE void core::array<type>::deallocate(core::array<type>& _array, bool destruct_elements) NOEXP {

	if (!_array._begin_) {
		_array._begin_ = nullptr;
		_array._end_ = nullptr;
		_array._count_ = 0;
		_array._size_ = 0;

		return;
	}

	// destruct elements if destructable 
	if (destruct_elements) {

		if constexpr (!std::is_trivially_destructible<type>::value) {
			// todo: maybe multi-threaded destruction if possible !!!
			for (type* obj = _array._begin_; obj != (_array._begin_ + _array._count_); obj++) {
				obj->~type();
			}
		}
	}

	// deallocate array memory
	if (_array._begin_) {
		_array._allocator_->deallocate(_array._handle_);
	}

	_array._begin_ = nullptr;
	_array._end_ = nullptr;
	_array._count_ = 0;
	_array._size_ = 0;
}

template<typename type>
INLINE void core::array<type>::allocate(core::array<type>& _array, u32 new_elements_count, bool construct_objects) NOEXP {

	new_elements_count = (new_elements_count) ? new_elements_count : 1;

	_array._count_ = new_elements_count;
	_array._size_ = sizeof(type) * _array._count_;

	// free current memory
	if (_array._begin_) core::array<type>::deallocate(_array, true);

	// reallocate new memory
	if (_array._allocator_) {
		_array._handle_ = _array._allocator_->allocate(memory_request{ .size = _array._size_ , ._tag_ = _array._size_ });
		_array._begin_ = (type*)_array._handle_.ptr;
		_array._end_ = _array._begin_ + _array._count_;
	}
	else {
		_array._begin_ = (type*)core::memory::allocate(_array._size_);
	}

	// construct objects "optional"
	if (construct_objects) {
		for (type* ptr = _array._begin_; ptr != _array._end_; ptr++) {
			new (ptr) type();
		}
	}

	// update array variables
	_array._end_ = (_array._begin_ + _array._count_);
}

#endif
#endif