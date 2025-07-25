#pragma once 

#ifndef MEMORY_CPP
#define MEMORY_CPP

#include "macros.hpp"

#ifdef WINDOWS
	// note: windef's to avoid conflict  with other type like byte,bool,...
	#ifndef __wtypes_h__
		#include <wtypes.h>
	#endif

	#ifndef __WINDEF_
		#include <windef.h>
	#endif

	#include <Windows.h>
#endif

#include "assert.hpp"
#include "errors.hpp"
#include "memory.hpp"

#include <array>
#include <sstream>
#include <map>

// allocated size for each section , in byte
static std::array<uint64_t , 8> sections_sizes = {
	NULL,// 0 , UNKOWN
	NULL,// 1 , GENERAL_PURPOSE
	NULL,// 2 , MODELS
	NULL,// 3 , TEXTURES
	NULL,// 4 , AUDIOS
	NULL,// 5 , ANIMATION
	NULL,// 6 , PHYSICS
	NULL // 7 , STDCPP
};

typedef struct alloc_info {
	size_t  size;
	uint8_t section;
};

namespace memory {
	// total allocated cpu memory in byte
	static u64 allocated_size = NULL;
	// map< pointer , alloc_size >
	static std::map < ptr, alloc_info > allocations_list = {};
}


/*
	memory namespace functions
*/
std::string pointer_to_hex_string(ptr64 pointer);

u64 memory::sizeof_section(ALLOCATION_SECTION section) noexcept {
	u8 _section = (u8)section;

	if (_section < sections_sizes.size()) {
		return sections_sizes[_section];
	}
	else return NULL;
}

static std::array<u32, 4> memory_factors = {
	1.0f , 1024.0f , 1048576.0f ,  1073741824.0f
};

double memory::sizeof_section_f(ALLOCATION_SECTION section, MEMORY_UNIT unit) noexcept {
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

double memory::total_size_f(MEMORY_UNIT unit) noexcept {
	if ((uint8_t)unit < 4) {
		return double(memory::allocated_size) / memory_factors[(uint8_t)unit];
	}
	else return double(memory::allocated_size);
}

// todo : handle multi-threaded allocations
void* memory::alloc(size_t size , ALLOCATION_SECTION section) {

	CRASH_AT_TRUE(size < 1, "memory::alloc zero size allocation not allowed !")
	
	void* pointer = new byte[size];

	CRASH_AT_TRUE( (pointer == nullptr) , "memory::alloc failed to allocate memory");

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

// TODO: move this to better place !
std::string pointer_to_hex_string(ptr64 pointer) {
	std::stringstream s_stream;
	s_stream << std::hex << pointer;

	return std::string( "0x" + s_stream.str());
}

// TODO : implementation for LINUX
memory_info memory::get_cpu_memory_info() noexcept{

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