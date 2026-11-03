#pragma once 

#ifndef CORE_GLOBAL_ARRAY_HPP
#define CORE_GLOBAL_ARRAY_HPP

#include "core/types.hpp"
#include "core/logger/logger.hpp"
#include "core/memory/memory.hpp"

#ifdef DEBUG
	static inline auto _carr_hpp_lgr_ = CORE_GET_LOGGER(DATA_STRUCTER_LOGGER);
#else 
	static inline auto _carr_hpp_lgr_ = nullptr;
#endif

namespace core {
/*
	global_array for simple and gloabl usage , used with global memory allocator core::memory .
	NOTE: there is no safety use it carefuly .
*/
template<typename type> class global_array {
	private:
		inline static const string _typename_ = TYPE_NAME(type);

		memory_handle_g _handle_;
		u32   _count_ = NULL;
		u64   _size_  = NULL;
		type* _begin_ = nullptr;

	public:
		// constructor
		global_array( ) = default;
		global_array( global_array<type> const& array_to_copy ) NOEXP;
		global_array( global_array<type> &&     array_to_move ) NOEXP;
		global_array(const u32 elements_count, subsystem_memory_tag tag) NOEXP;

		template<typename... args> global_array(const u32 elements_count, subsystem_memory_tag tag, args&&... arguments) NOEXP;
		
		// operators
		type& operator[] (u32 index) NOEXP;
		
		type& operator=(global_array<type> const& array_to_copy) NOEXP; // copy
		type& operator=(global_array<type> &&     array_to_move) NOEXP; // move
		
		// destructor
		~global_array();
		
		// functions
		u64 size()    NOEXP { return _size_; }
		u32 capacity() NOEXP { return _count_; }
		
		type* begin() NOEXP { return _begin_; }
		type* end()    NOEXP { return (_begin_ + _count_); }

		subsystem_memory_tag tag() NOEXP { return _handle_.tag(); }

	}; // calss global_array end

} // namespace core end

#endif