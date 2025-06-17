
#pragma once 

#ifndef MACROS_HPP
#define MACROS_HPP

// maybe (INTPTR_MAX == INT64_MAX) for linux 
#ifdef _WIN64 or WIN64 or __x86_64__ or _____LP64_____
	#define X64 // if arch is 64bit
#else 
	#define X32 // if arch is 32bit
#endif

// for windows 
#ifdef _WIN32 or _WIN64 or WIN32 or WIN64 or WIN or __WINDOWS__ or __WIN32__ or 
	#define WINDOWS
#endif 

// for linux
#ifdef __GNU__ or __gnu_hurd__ or __gnu_linux__ or __linux__ or linux or __linux
	#define LINUX
#endif

// compile-time failure for undetected systems
#ifndef WINDOWS or LINUX
	static_assert(0, "no macro detected for your current operation system :: " __FILE__);
#endif

#endif // !MACROS_HPP
