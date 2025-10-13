#pragma once 

#ifndef MEMORY_CPP
#define MEMORY_CPP

#include "core/macros.hpp"

#ifdef WINDOWS
	// note: windef's to avoid conflict with other type like byte,bool,...
	#ifndef __wtypes_h__
		#include <wtypes.h>
	#endif

	#ifndef __WINDEF_
		#include <windef.h>
	#endif

	#include <Windows.h>
#endif

#include <array>
#include <sstream>
#include <map>

#include "core/assert.hpp"
#include "core/status/status.hpp"
#include "core/logger/logger.hpp"

#include "memory.hpp"

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

static std::array<u32, 4> memory_factors = {
	1.0f , 1024.0f , 1048576.0f ,  1073741824.0f
};


typedef struct alloc_info {
	size_t  size;
	uint8_t section;
};

namespace core::memory {
	// total allocated cpu memory in byte
	static u64 allocated_size = NULL;
	// map< pointer , alloc_size >
	static std::map < ptr, alloc_info > allocations_list = {};
}



/*
	memory namespace functions
*/
u64 core::memory::sizeof_section(ALLOCATION_SECTION section) noexcept {
	u8 _section = (u8)section;

	if (_section < sections_sizes.size()) {
		return sections_sizes[_section];
	}
	else return NULL;
}


double core::memory::sizeof_section_f(ALLOCATION_SECTION section, MEMORY_UNIT unit) noexcept {
	uint8_t _section = (uint8_t)section;

	if (_section < sections_sizes.size()) {
		if ((uint8_t)unit < 4) {
			return sections_sizes[_section] / memory_factors[(uint8_t)unit];
		}
		else return double(sections_sizes[_section]);
	}
	else return NULL;
}

uint64_t core::memory::total_size() noexcept {
	return core::memory::allocated_size;
}

double core::memory::total_size_f(MEMORY_UNIT unit) noexcept {
	if ((uint8_t)unit < 4) {
		return double(core::memory::allocated_size) / memory_factors[(uint8_t)unit];
	}
	else return double(core::memory::allocated_size);
}

// todo : handle multi-threaded allocations
void* core::memory::alloc(size_t size , ALLOCATION_SECTION section) {

	CRASH_IF(size < 1, "core::memory::alloc zero size allocation not allowed !")
	
	void* pointer = new byte[size];

	CRASH_IF( (pointer == nullptr) , "core::memory::alloc failed to allocate memory");

	// update total size
	core::memory::allocated_size += size;

	// update section size
	uint8_t _section = (uint8_t)section;
	if (_section < sections_sizes.size()) {
		sections_sizes[_section] += size;
	}
	else sections_sizes[0] += size;

	// update alloc list
	core::memory::allocations_list.insert({ 
		(ptr)pointer , alloc_info{size , _section}
	});
	
	return pointer;
}

// todo : handle multi-threaded deallocations
void core::memory::free(void* pointer) {
	
	CRASH_IF(pointer == nullptr , "illegal core::memory::free null-pointer !");
		
	// check alloc_list
	auto allocation = core::memory::allocations_list.find((ptr64)pointer);

	CRASH_IF(
		allocation == core::memory::allocations_list.end(),
		"core::memory::free invalid pointer " + pointer_to_hex_string((ptr64)pointer)
	);

	// update total size
	core::memory::allocated_size = (core::memory::allocated_size - allocation->second.size) < 0 ? 0 : core::memory::allocated_size - allocation->second.size;
	
	// update section size
	uint8_t _section = allocation->second.section;
	if (_section < sections_sizes.size()) {
		sections_sizes[_section] = (sections_sizes[_section] - allocation->second.size) < 0 ? 0 : (sections_sizes[_section] - allocation->second.size);
	}
	else {
		// out of range _section mean there's a bug in core::memory::alloc
		// so we need assert crash
		CORE_ASSERT(true,"core::memory::alloc --> out of range _section mean there's a bug !!!");
	}

	// update alloc list
	core::memory::allocations_list.erase(allocation->first);

	delete[] pointer;
}

// TODO: move this to the right place !!!!!!!!!!!!!!!
std::string pointer_to_hex_string(ptr64 pointer) {
	std::stringstream s_stream;
	s_stream << std::hex << pointer;

	return std::string( "0x" + s_stream.str());
}

// TODO : implementation for LINUX
memory_info core::memory::get_cpu_memory_info() noexcept{

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