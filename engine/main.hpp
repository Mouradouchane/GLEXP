#pragma once

#ifndef MAIN_HPP
#define MAIN_HPP

#include "core/macros.hpp"
#include "core/targetver.h"

#ifdef WINDOWS
	#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows 
#endif

// C RunTime Header Files
#include "engine/icon/resource.h"

#ifdef WINDOWS
	// Windows Header Files
	#include <windows.h>
#endif

#include "core/assert.hpp"
#include "engine/application/application.hpp"

#endif
