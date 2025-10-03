#pragma once

#include "core/macros.hpp"

#ifdef WINDOWS
    #include "engine/icon/resource.h"
    #define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows
    // Windows Header Files
	#include <SDKDDKVer.h>
    #include <windows.h>
#endif

#include "engine/engine/engine.hpp"

#ifdef WINDOWS
int APIENTRY wWinMain(
    _In_ HINSTANCE h_instance, _In_opt_ HINSTANCE h_prev_instance, _In_ LPWSTR lp_cmd_line, _In_ int n_cmd_show
)
#else
int main(
    int argc, char* argv[]
)
#endif
{

    CORE_ASSERT(engine::init() != core::error::none, "failed to init engine");

    engine::run();

    engine::shutdown();

    return int(core::error::none);
}
