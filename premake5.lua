
-- generate ide solution
print('\27[34m' .. "============ BUILDING SOLUTION ============" .. '\27[0m')
 
build_path = "$(SolutionDir)build"
libs_path   = "$(SolutionDir)libs"

-- workspace/solution setup
workspace("glexp_workspace") 
    configurations{ 
        "debug" , "release"
    }

-- exe output folder
bindirs(build_path)

-- include dirs
includedirs{ "project/**" }
includedirs{ "libs" }
--includedirs{ "build/**" }

architecture("x64")

-- project setup
project("glexp_project")

files {
		"**.cpp" , "**.hpp" , "**.h" , "**.c" , 
		"**.vert" , "**.frag","**.ini" , "**.lua"
} 
files {"./build/shaders/**"}
files {"./build/textures/**"}
files {"./build/models/**"}

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

-- debugging working path
debugging_path = "$(SolutionDir)build"

assimp_dll_lib_path = libs_path.."/assimp/assimp-vc143-mt.lib"
-- hwinfo_lib_path = libs_path.."/hwinfo/"

filter("configurations:release")
    -- libs dirs "dynamic linking"
    links{ "opengl32.lib" }
	-- glew / glfw
    links{ libs_path.."/glew/glew32.lib" }
    links{ libs_path.."/glfw/glfw3dll.lib" }
	-- hwinfo
    --links{ hwinfo_lib_path.."hwinfo_ram.lib" }
	-- assimp
    links{ assimp_dll_lib_path }
	-- release configs
    targetname "glexp_rx64"
    defines {"NDEBUG"}
    debugdir(debugging_path) 
    symbols("Off")
    optimize "Off"    -- TODO: ON LATER

filter("configurations:debug")
    -- libs dirs "static linking"
    links{ "opengl32.lib" }
	-- glew / glfw
    links{ libs_path.."/glew/glew32s.lib" }
    links{ libs_path.."/glfw/glfw3.lib" }
	-- hwinfo
	-- links{ hwinfo_lib_path.."hwinfo_ram.lib" }
	-- assimp
    links{ assimp_dll_lib_path }
	-- debug configs
    targetname "glexp_dx64"
    defines("GLEW_STATIC") -- for glew static linking
    debugdir(debugging_path) 
    defines { "DEBUG" }
    symbols("On")
    optimize "Off"
	
-- disable few warning related to libs
-- disablewarnings( "C26812" ) -- assimp 
-- disablewarnings( "C26451" ) -- assimp
