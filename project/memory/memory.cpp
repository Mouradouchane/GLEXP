#pragma once 

#ifndef MEMORY_CPP
#define MEMORY_CPP

#include "macros.hpp"
#include "assert.hpp"
#include "errors.hpp"
#include "memory.hpp"

#include <sstream>
#include <map>
#ifdef WINDOWS
	#include <Windows.h>
#endif

namespace memory {	
	// total allocated/used cpu memory in BYTE
	static uint64_t allocated_size = NULL;

	// map< pointer , alloc_size >
	static std::map<ptr_64, size_t> allocations_list = {};
}

/*
	memory namespace functions
*/

uint64_t memory::allocated_memory_size() {
	return memory::allocated_size;
}

// todo: implement custom allocation
void* memory::alloc(size_t size) {

	CRASH_AT_FALSE(size , "memory::alloc zero size allocation not allowed !")
	
	void* pointer = new int8_t[size];

	CRASH_AT_FALSE( (pointer != nullptr) , "memory::alloc failed to allocate memory");

	memory::allocated_size += size;
	memory::allocations_list.insert( { (ptr_64)pointer , size } );
	
	return pointer;
}

// todo: free memory based on look_up table
void memory::free(void* pointer ) {
	
	CRASH_AT_FALSE( (pointer != nullptr) , "illegal memory::free null-pointer !");

	// check alloc_list
	auto allocation = memory::allocations_list.find((ptr_64)pointer);

	CRASH_AT_FALSE(
		(allocation != memory::allocations_list.end()),
		"memory::free invalid pointer " + pointer_to_hex_string((ptr_64)pointer)
	);

	// update list
	memory::allocated_size = (memory::allocated_size - allocation->second) < 0 ? 0 : memory::allocated_size - allocation->second;
	memory::allocations_list.erase(allocation->first);

	delete[] pointer;
}

/*
	convert to byte functions
*/

uint64_t kb_to_byte(size_t KB) noexcept {
	return KB * 1024;
}

uint64_t mb_to_byte(size_t MB) noexcept {
	return MB * 1048576;
}

uint64_t gb_to_byte(size_t GB) noexcept {
	return GB * 1073741824;
}

double byte_to_kb(size_t byte) noexcept {
	return double(double(byte)/1024.0f);
}

double byte_to_mb(size_t byte) noexcept{
	return double(double(byte)/1048576.0f);
}

double byte_to_gb(size_t byte) noexcept{
	return double(double(byte)/1073741824.0f);
}

std::string pointer_to_hex_string(ptr_64 pointer) {
	std::stringstream s_stream;
	s_stream << std::hex << pointer;

	return std::string( "0x" + s_stream.str());
}

// TODO : implement LINUX
memory_info memory::get_cpu_memory_info() {
	DEBUG_BREAK;

#ifdef WINDOWS
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex); 

	if (GlobalMemoryStatusEx(&statex)) {
		return memory_info{
			statex.ullTotalPhys,
			statex.ullAvailPhys
		};
	}
	else return memory_info();

#elif LINUX
	ASSERT_EXP(0);
#else
	ASSERT_EXP(0);
#endif

}

#endif