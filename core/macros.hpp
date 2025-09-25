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
#ifdef _WIN64 || WIN64 || __x86_64__ || _____LP64_____
	#define X64
#else 
	#define X32
#endif

// windows
#ifdef _WIN32 || _WIN64 || WIN32 || WIN64 || WIN || __WINDOWS__ || __WIN32__
	#define WINDOWS
#endif 

// linux
#ifdef __GNU__ || __gnu_hurd__ || __gnu_linux__ || __linux__ || linux || __linux
	#define LINUX
#endif

// when systems is unsupported
#ifndef WINDOWS || LINUX
	static_assert(0, "no macro detected f|| your current operation system :: " __FILE__);
#endif

// warning macros
#define DISABLE_WARNING_START __pragma(warning(push,0))
#define DISABLE_WARNING(WARNINGS) __pragma(warning(disable,WARNINGS))
#define DISABLE_WARNING_END   __pragma(warning(pop))

#endif // !MACROS_HPP
