
-- generate ide solution
print('\27[34m' .. "============ BUILDING SOLUTION ============" .. '\27[0m')
 
build_path = "$(SolutionDir)build"
libs_path  = "$(SolutionDir)project/libs"

-- workspace/solution setup
workspace("glexp_workspace") 
    configurations{ 
        "debug_static" , "debug_dynamic",
        "release_static", "release_dynamic"
    }

-- exe output folder
bindirs(build_path)
-- include dirs
includedirs{ "./project/**" }
includedirs{ "./build/**" }

architecture("x64")
-- project setup
project("glexp_project")

files {"**.cpp" , "**.hpp" , "**.h" , "**.c" , "**.vert" , "**.frag","**.ini" , "**.lua"} 

-- program icon
filter { "system:windows" }
    icon("./projec/icon.ico")
    files { "./project/common/resource.rc"}
    --vpaths { ['Resources/*'] = { '*.rc', '**.ico' }


-- project type
kind("WindowedApp")
language "C++"

targetdir("$(SolutionDir)build") -- exe output folder
objdir(build_path.."/binaries/") -- obj output folder

filter("configurations:debug_dynamic")
    -- libs dirs "dynamic linking"
    links{ "opengl32.lib" }
    links{ libs_path.."/glew/glew32.lib" }
    links{ libs_path.."/glfw/glfw3dll.lib" }
    targetname "glexp_dx64"
    defines { "DEBUG" }
    symbols("On")

filter("configurations:release_dynamic")
    -- libs dirs "dynamic linking"
    links{ "opengl32.lib" }
    links{ libs_path.."/glew/glew32.lib" }
    links{ libs_path.."/glfw/glfw3dll.lib" }
    targetname "glexp_rx64"
    defines {"NDEBUG"}
    symbols("Off")
    optimize "Off"

filter("configurations:debug_static")
    -- libs dirs "static linking"
    links{ "opengl32.lib" }
    links{ libs_path.."/glew/glew32s.lib" }
    links{ libs_path.."/glfw/glfw3.lib" }
    targetname "glexp_dx64"
    defines("GLEW_STATIC") -- for glew static linking
    defines { "DEBUG" }
    symbols("On")

filter("configurations:release_static")
    -- libs dirs "static linking"
    links{ "opengl32.lib" }
    links{ libs_path.."/glew/glew32s.lib" }
    links{ libs_path.."/glfw/glfw3.lib" }
    targetname "glexp_rx64"
    defines("GLEW_STATIC") -- for glew static linking
    defines {"NDEBUG"}
    symbols("Off")
    optimize "Off"


