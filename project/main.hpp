// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include "common/targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files

#include <windows.h>
// C RunTime Header Files
#include "common/resource.h"

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "glew/glew.h"
#include "glfw/glfw3.h"

#include "config/config.hpp"
#include "common/globals.hpp"
#include "input/inputs.hpp"
#include "textures/texture_2d.hpp"
#include "shaders/shader.hpp"

#endif
