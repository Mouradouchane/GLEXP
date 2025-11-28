#pragma once 

#ifndef CORE_GLOBAL_MEMORY_CPP
#define CORE_GLOBAL_MEMORY_CPP

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

#include "global_memory.hpp"

// allocated count for each section , in byte
static std::array<uint64_t , 8> sections_sizes = {
	NULL,// 0 , unkown
	NULL,// 1 , GENERAL_PURPOSE
	NULL,// 2 , models
	NULL,// 3 , textures
	NULL,// 4 , audios
	NULL,// 5 , animations
	NULL,// 6 , physics
	NULL // 7 , stdcpp
};

static const std::array<u32, 4> memory_factors = {
	1u ,			// BYTE
	1024u ,			// KB
	1048576u ,		// MB
	1073741824u		// GB
};


typedef struct alloc_info {
	size_t count;
	u8     section;
};

namespace core::global_memory {
	// total allocated cpu global_memory in byte
	static u64 allocated_size = NULL;
	// map< pointer , alloc_size >
	static std::map < ptr, alloc_info > allocations_list = {};
}



/*
	global_memory namespace functions
*/
u64 core::global_memory::sizeof_section(memory_usage section) noexcept {
	u8 _section = (u8)section;

	if (_section < sections_sizes.size()) {
		return sections_sizes[_section];
	}
	else return NULL;
}


f64 core::global_memory::sizeof_section_f(memory_usage section, memory_unit unit) noexcept {
	uint8_t _section = (uint8_t)section;

	if (_section < sections_sizes.size()) {
		if ((uint8_t)unit < 4) {
			 return f64(sections_sizes[_section] / memory_factors[(uint8_t)unit]);
		}
		else return f64(sections_sizes[_section]);
	}
	else return NULL;
}

uint64_t core::global_memory::total_size() noexcept {
	return core::global_memory::allocated_size;
}

double core::global_memory::total_size_f(memory_unit unit) noexcept {
	if ((uint8_t)unit < 4) {
		return double(core::global_memory::allocated_size) / memory_factors[(uint8_t)unit];
	}
	else return double(core::global_memory::allocated_size);
}

// todo : handle multi-threaded allocations
void* core::global_memory::allocate(size_t count , memory_usage section) {

	CRASH_IF(count < 1, "core::global_memory::allocate zero count allocation not allowed !")
	
	void* pointer = new byte[count];

	CRASH_IF( (pointer == nullptr) , "core::global_memory::allocate failed to allocate global_memory");

	// update total count
	core::global_memory::allocated_size += count;

	// update section count
	uint8_t _section = (uint8_t)section;
	if (_section < sections_sizes.size()) {
		sections_sizes[_section] += count;
	}
	else sections_sizes[0] += count;

	// update allocate list
	core::global_memory::allocations_list.insert({ 
		(ptr)pointer , alloc_info{count , _section}
	});
	
	return pointer;
}

// todo : handle multi-threaded deallocations
void core::global_memory::deallocate(void* pointer) {
	
	CRASH_IF(pointer == nullptr , "illegal core::global_memory::deallocate null-pointer !");
		
	// check alloc_list
	auto allocation = core::global_memory::allocations_list.find((ptr64)pointer);

	CRASH_IF(
		allocation == core::global_memory::allocations_list.end(),
		"core::global_memory::deallocate invalid pointer " + pointer_to_hex_string((ptr64)pointer)
	);

	// update total count
	core::global_memory::allocated_size = (core::global_memory::allocated_size - allocation->second.count) < 0 ? 0 : core::global_memory::allocated_size - allocation->second.count;
	
	// update section count
	uint8_t _section = allocation->second.section;
	if (_section < sections_sizes.size()) {
		sections_sizes[_section] = (sections_sizes[_section] - allocation->second.count) < 0 ? 0 : (sections_sizes[_section] - allocation->second.count);
	}
	else {
		// out of range _section mean there's a bug in core::global_memory::allocate
		// so we need assert crash
		CORE_ASSERT(true,"core::global_memory::allocate --> out of range _section mean there's a bug !!!");
	}

	// update allocate list
	core::global_memory::allocations_list.erase(allocation->first);

	delete[] pointer;
}

// TODO: move this to the right place !!!!!!!!!!!!!!!
std::string pointer_to_hex_string(ptr64 pointer) {
	std::stringstream s_stream;
	s_stream << std::hex << pointer;

	return std::string( "0x" + s_stream.str());
}

// TODO : implementation for LINUX
memory_info core::global_memory::get_cpu_memory_info() noexcept{

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