#pragma once

#ifndef MAIN_ENTRY_CPP
#define MAIN_ENTRY_CPP

#include "core/macros.hpp"

#ifdef WINDOWS
    #include "engine/icon/resource.h"
    
    // exclude rarely-used stuff from windows
    #define WIN32_LEAN_AND_MEAN
    // windows header files
	#include <SDKDDKVer.h>
    #include <windows.h>
#endif

#include "core/logger/logger.hpp"
#include "engine/engine.hpp"

// todo: create and enable logger
#define _LOGGER_ nullptr

#ifdef WINDOWS
    int APIENTRY wWinMain(
        _In_ HINSTANCE h_instance, _In_opt_ HINSTANCE h_prev_instance, _In_ LPWSTR lp_cmd_line, _In_ int n_cmd_show
    )
#else
    int main( int argc, char* argv[] )
#endif
{

    CORE_FATAL_IF(engine::init(), 0, ENGINE_INIT_FAILED);

    engine::run();

    engine::shutdown();

    return 0;
}

#endif