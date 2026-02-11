#pragma once 

#ifndef MACROS_HPP
#define MACROS_HPP

/*
	NOTE: if youre not using visual studio or windows 
	make sure to change __debugbreak(); with the one working for your IDE/DEBUGGER
*/ 
#ifdef DEBUG
	#include <intrin.h>
	#define DEBUG_BREAK __debugbreak();
#else	
	#define DEBUG_BREAK
#endif

// for class/interface
#define NEED_IMPL 0

// current arch x64 or x32 maybe other arch later 
#if defined(_WIN64) || defined(WIN64) || defined(__x86_64__) || defined(_____LP64_____)
	#define X64
#else
	#define X32
#endif

// windows
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64) || defined(WIN) || defined(__WINDOWS__) || defined(__WIN32__)
	#define WINDOWS
// linux
#elif defined(__GNU__) || defined(__gnu_hurd__) || defined(__gnu_linux__) || defined(__linux__) || defined(linux) || defined(__linux)
	#define LINUX
#endif

// when systems is unsupported
#if defined(WINDOWS) || defined(LINUX)

#else
	static_assert(0, "compile-time-assert --> unsupported operation system , " __LINE__ " " __FILE__);
#endif

// warning macros
#define DISABLE_WARNING_START __pragma(warning(push,0));
#define DISABLE_WARNING_END   __pragma(warning(pop));

#if defined(UNIT_TEST)
	#define CORE_CRASH()
#else
	#define CORE_CRASH() exit(-1);
#endif

/*
	dynamic library macros
*/
#ifdef DLL_EXPORT
	#if defined(WINDOWS)
		#define DLL_API       extern "C"       __declspec(dllexport)
		#define DLL_API_CLASS extern "C" class __declspec(dllexport)
		#define DLL_API_INTERFACE extern "C" class __declspec(dllexport)

	#elif defined(LINUX)
		#define DLL_API       extern "C"       __attribute__((visibility("default")))
		#define DLL_API_CLASS extern "C" class __attribute__((visibility("default")))
		#define DLL_API_INTERFACE extern "C" class __attribute__((visibility("default")))

	#else
		static_assert(0, "compile-time-error --> unsupported operation system , line:" __LINE__ " file:" __FILE__);
	#endif

#else
	#ifdef DLL_IMPORT
		#define DLL_API       extern "C"       __declspec(dllimport)
		#define DLL_API_CLASS extern "C" class __declspec(dllimport)
		#define DLL_API_INTERFACE extern "C" class __declspec(dllimport)
	#else 
		#define DLL_API
		#define DLL_API_CLASS class
		#define DLL_API_INTERFACE class
	#endif
#endif


/*
	few macros to convert beetween memory units
*/
// convert "kb,mb,gb" to "BYTES"
#define KB_TO_BYTE(_KB) u64(_KB) * 1024u
#define MB_TO_BYTE(_MB) u64(_MB) * 1048576u
#define GB_TO_BYTE(_GB) u64(_GB) * 1073741824u

// example: 2 KB --> 2 *1024u
#define KB   *1024u
#define MB   *1048576u
#define GB   *1073741824u

// convert "BYTES" to "kb,mb,gb"
#define BYTE_TO_KB(_BYTE) (_BYTE / 1024.0f)
#define BYTE_TO_MB(_BYTE) (_BYTE / 1048576.0f)
#define BYTE_TO_GB(_BYTE) (_BYTE / 1073741824.0f)
// ===================================================


#endif // MACROS_HPP
