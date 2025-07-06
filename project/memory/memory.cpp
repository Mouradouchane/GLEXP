#pragma once 

#ifndef MEMORY_CPP
#define MEMORY_CPP

#include "macros.hpp"
#include "assert.hpp"
#include "errors.hpp"
#include "memory.hpp"

#include <array>
#include <sstream>
#include <map>
#ifdef WINDOWS
	#include <Windows.h>
#endif

// allocated size for each section , in BYTE
static std::array<uint64_t , 8> sections_sizes = {
	NULL,// 0 , UNKOWN
	NULL,// 1 , GENERAL_PURPOSE
	NULL,// 2 , MODELS
	NULL,// 3 , TEXTURES
	NULL,// 4 , AUDIOS
	NULL,// 5 , ANIMATION
	NULL // 6 , PHYSICS
};

typedef struct alloc_info {
	size_t  size;
	uint8_t section;
};

// private for main "memory allocator" namespace 
namespace memory {
	
	// total allocated cpu memory in BYTE
	static uint64_t allocated_size = NULL;

	// map< pointer , alloc_size >
	static std::map < ptr, alloc_info > allocations_list = {};
}

/*
	memory namespace functions
*/

uint64_t memory::sizeof_section(allocation_section section) noexcept {
	uint8_t _section = (uint8_t)section;

	if (_section < sections_sizes.size()) {
		return sections_sizes[_section];
	}
	else return NULL;
}

static std::array<uint32_t, 4> memory_factors = {
	1.0f , 1024.0f , 1048576.0f ,  1073741824.0f
};

double memory::sizeof_section_f(allocation_section section, memory_unit unit) noexcept {
	uint8_t _section = (uint8_t)section;

	if (_section < sections_sizes.size()) {
		if ((uint8_t)unit < 4) {
			return sections_sizes[_section] / memory_factors[(uint8_t)unit];
		}
		else return double(sections_sizes[_section]);
	}
	else return NULL;
}

uint64_t memory::total_size() noexcept {
	return memory::allocated_size;
}

double memory::total_size_f(memory_unit unit) noexcept {
	if ((uint8_t)unit < 4) {
		return double(memory::allocated_size) / memory_factors[(uint8_t)unit];
	}
	else return double(memory::allocated_size);
}

// todo : handle multi-threaded allocations
void* memory::alloc(size_t size , allocation_section section) {

	CRASH_AT_FALSE(size , "memory::alloc zero size allocation not allowed !")
	
	void* pointer = new int8_t[size];

	CRASH_AT_FALSE( (pointer != nullptr) , "memory::alloc failed to allocate memory");

	// update total size
	memory::allocated_size += size;

	// update section size
	uint8_t _section = (uint8_t)section;
	if (_section < sections_sizes.size()) {
		sections_sizes[_section] += size;
	}
	else sections_sizes[0] += size;

	// update alloc list
	memory::allocations_list.insert({ 
		(ptr)pointer , alloc_info{size , _section} 
	});
	
	return pointer;
}

// todo : handle multi-threaded deallocations
void memory::free(void* pointer) {
	
	CRASH_AT_FALSE( (pointer != nullptr) , "illegal memory::free null-pointer !");
		
	// check alloc_list
	auto allocation = memory::allocations_list.find((ptr64)pointer);

	CRASH_AT_FALSE(
		(allocation != memory::allocations_list.end()),
		"memory::free invalid pointer " + pointer_to_hex_string((ptr64)pointer)
	);

	// update total size
	memory::allocated_size = (memory::allocated_size - allocation->second.size) < 0 ? 0 : memory::allocated_size - allocation->second.size;
	
	// update section size
	uint8_t _section = allocation->second.section;
	if (_section < sections_sizes.size()) {
		sections_sizes[_section] = (sections_sizes[_section] - allocation->second.size) < 0 ? 0 : (sections_sizes[_section] - allocation->second.size);
	}
	else {
		// out of range _section mean there's a bug in memory::alloc
		// so we need assert crash
		ASSERT_EXP(0);
	}

	// update alloc list
	memory::allocations_list.erase(allocation->first);

	delete[] pointer;
}

std::string pointer_to_hex_string(ptr64 pointer) {
	std::stringstream s_stream;
	s_stream << std::hex << pointer;

	return std::string( "0x" + s_stream.str());
}

// TODO : implementation for LINUX
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