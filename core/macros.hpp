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

#define CORE_CRASH() exit(-1);

/*
	dynamic library macros
*/
#ifdef DLL_EXPORT
	#if defined(WINDOWS)
		#define DLL_API extern "C" __declspec(dllexport)
	#elif defined(LINUX)
		#define DLL_API extern "C" __attribute__((visibility("default")))
	#else
		static_assert(0, "compile-time-error --> unsupported operation system , line:" __LINE__ " file:" __FILE__);
	#endif

#else
	#ifdef DLL_IMPORT
		#define DLL_API extern "C" __declspec(dllimport)
	#else 
		#define DLL_API
	#endif
#endif

#endif // MACROS_HPP
