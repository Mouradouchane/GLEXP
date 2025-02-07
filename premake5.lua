/
-- workspace/solution setup
workspace("glexp_workspace") 
    configurations{ 
        "debug_static" , "debug_dynamic",
        "release_static", "release_dynamic"
    }
    bindirs("$(SolutionDir)build")

architecture("x64")
-- project setup
project("glexp_project")

files {"**.cpp" , "**.hpp" , "**.h" , "**.c" , "**.vert" , "**.frag","**.ini"} 

-- program icon
filter { 'system:windows' }
    icon("./projec/icon.ico")
    files { "./project/common/resource.rc"}
    --vpaths { ['Resources/*'] = { '*.rc', '**.ico' }

-- include dirs
includedirs{ "./libs/" }
includedirs{ "./project/**" }
includedirs{ "./build/shaders/**" }


-- project type
kind("WindowedApp")
language "C++"

targetdir("build/") -- exe output folder
objdir("build/binaries/") -- obj output folder
  

filter("configurations:debug_dynamic")
    -- libs dirs "dynamic linking"
    links{ "opengl32.lib" }
    links{ "$(SolutionDir)libs/glew/glew32.lib" }
    links{ "$(SolutionDir)libs/glfw/glfw3dll.lib" }
    targetname "glexp_dx64"
    defines { "DEBUG" }
    symbols("On")

filter("configurations:debug_static")
    -- libs dirs "static linking"
    links{ "opengl32.lib" }
    links{ "$(SolutionDir)libs/glew/glew32s.lib" }
    links{ "$(SolutionDir)libs/glfw/glfw3.lib" }
    targetname "glexp_dx64"
    defines("GLEW_STATIC") -- for glew static linking
    defines { "DEBUG" }
    symbols("On")

filter("configurations:release_static")
    -- libs dirs "static linking"
    links{ "opengl32.lib" }
    links{ "$(SolutionDir)libs/glew/glew32s.lib" }
    links{ "$(SolutionDir)libs/glfw/glfw3.lib" }
    targetname "glexp_rx64"
    defines("GLEW_STATIC") -- for glew static linking
    defines {"NDEBUG"}
    symbols("Off")
    optimize "Off"

filter("configurations:release_dynamic")
    -- libs dirs "dynamic linking"
    links{ "opengl32.lib" }
    links{ "$(SolutionDir)libs/glew/glew32.lib" }
    links{ "$(SolutionDir)libs/glfw/glfw3dll.lib" }
    targetname "glexp_rx64"
    defines {"NDEBUG"}
    symbols("Off")
    optimize "Off"
