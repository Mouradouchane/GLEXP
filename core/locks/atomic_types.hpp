#pragma once 

#ifndef CORE_ATOMIC_TYPES_HPP
#define CORE_ATOMIC_TYPES_HPP

#include <atomic>

#include "core/types.hpp"

// pointers
typedef std::atomic<ptr>   atomic_ptr;
typedef std::atomic<ptr8>  atomic_ptr8;
typedef std::atomic<ptr16> atomic_ptr16;
typedef std::atomic<ptr32> atomic_ptr32;
typedef std::atomic<ptr64> atomic_ptr64;

// uint's
typedef std::atomic<u8>  atomic_u8;
typedef std::atomic<u16> atomic_u16;
typedef std::atomic<u32> atomic_u32;
typedef std::atomic<u64> atomic_u64;

// indexes 
typedef std::atomic<index8>  atomic_index8;
typedef std::atomic<index16> atomic_index16;
typedef std::atomic<index32> atomic_index32;
typedef std::atomic<index64> atomic_index64;

// id's
typedef std::atomic<id8>  atomic_id8;
typedef std::atomic<id16> atomic_id16;
typedef std::atomic<id32> atomic_id32;
typedef std::atomic<id64> atomic_id64;

#endif