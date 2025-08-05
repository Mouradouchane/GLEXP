#pragma once 

#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <string>
#include "macros.hpp"
#include "types.hpp"

enum class ALLOCATION_SECTION : uint8_t {
    UNKOWN    = 0,
	GENERAL   = 1,
	MODELS    = 2,
	TEXTURES  = 3,
	AUDIOS    = 4,
	ANIMATION = 5,
	PHYSICS   = 6,
    STDCPP    = 7,
};

enum class MEMORY_UNIT : u8 {
	byte = 0,  kb = 1,  mb = 2,  gb = 3
};

struct memory_info {
	u64 size = NULL;
	u64 free = NULL;
};

// just text representation
#define KB *1
#define MB *1
#define GB *1

// convert "kb,mb,gb" to "BYTES" macros
#define KB_TO_BYTE(_kb) uint64_t(_kb) * 1024
#define MB_TO_BYTE(_mb) uint64_t(_mb) * 1048576
#define GB_TO_BYTE(_gb) uint64_t(_gb) * 1073741824

// convert "BYTES" to "kb,mb,gb" macros
#define BYTE_TO_KB(_bytes) (_bytes/1024.0f)
#define BYTE_TO_MB(_bytes) (_bytes/1048576.0f)
#define BYTE_TO_GB(_bytes) (_bytes/1073741824.0f)

// TODO: move this to the right place !!!!!!!!!!!!!!!
std::string pointer_to_hex_string(ptr64 pointer);

/*
	- main "general purpose" memory allocator
	- used by other custom allocators => heap,pool,arena,...
*/
namespace memory {

	void* alloc(size_t size, ALLOCATION_SECTION section = ALLOCATION_SECTION::UNKOWN);
	void  free(void* pointer);

	uint64_t total_size() noexcept;
	double   total_size_f(MEMORY_UNIT unit) noexcept;
	uint64_t sizeof_section(ALLOCATION_SECTION section) noexcept;
	double   sizeof_section_f(ALLOCATION_SECTION section , MEMORY_UNIT unit) noexcept;

	// todo : move it away from namespace memory
	memory_info get_cpu_memory_info() noexcept;

} // namespace memory end

/*
	used to allocate/deallocate memory for std library 
*/
template<class T> struct custom_allocator {

    typedef T value_type;

	 custom_allocator() = default;
	~custom_allocator() = default;

    template<class elm_type> constexpr custom_allocator(
		const custom_allocator <elm_type>&
	) noexcept {
	
	}
	
    T* allocate(std::size_t size) {
		return (T*)memory::alloc(size * sizeof(T), ALLOCATION_SECTION::STDCPP);
    }

    void deallocate(T* pointer, std::size_t size) {
		memory::free(pointer);
	}

	template<typename elm_type, typename... Args> void construct(
		elm_type* p, Args&&... args
	) {
		new(p) elm_type( std::forward<Args>(args)... );
	}

	// destructor's caller
	template<typename obj> void destroy(obj* pointer) {
		pointer->~obj();
	}

};
// struct custom_allocator end

#endif