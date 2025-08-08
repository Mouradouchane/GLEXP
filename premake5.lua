
-- generate ide solution
print('\27[34m' .. "============ BUILDING SOLUTION ============" .. '\27[0m')
 
-- ============ ============ ============ 
-- ============ workspace paths ============== 
build_path  = "$(SolutionDir)build"
libs_path    = "$(SolutionDir)libs"
tester_path = "$(SolutionDir)tester"

assimp_dll_lib_path = libs_path.."/assimp/assimp-vc143-mt.lib"

-- working paths
debugging_path = "$(SolutionDir)build"
release_path      = "$(SolutionDir)build"
-- ============ ============ ============ 
-- ============ ============ ============ 

-- ============ ============ ============ 
-- ============ workspace setup ============
workspace("glexp_workspace") 
    configurations{ 
        "debug" , "release"
    }
-- ============ ============ ============ 
	
	
-- ============ ============ ============ 
-- ============ glexp project setup ============ 
project("glexp_project")

	-- exe output folder
	bindirs(build_path)
	
	-- include dirs
	includedirs{ "project/**" }
	includedirs{ "libs" }

	architecture("x64")

	-- project source folders
	files {"project/**"}
	files {"libs/**"}
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

	-- exe/obj output folder
	targetdir(build_path) 
	objdir(build_path.."/binaries/") 


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
-- ==============================
-- ==============================


-- ============ ============ ============ 
-- ============ tester project setup ============
project("tester")

	-- project  config
	kind("ConsoleApp")
	language "C++"
	architecture("x64")

	-- build output path
	targetdir(build_path) 
	objdir(build_path.."/binaries/") 

	-- include dirs
	files {
			"tester/**",
			"libs/ftxui/**"
	} 
	includedirs{ "$(SolutionDir)tester/**" , "$(SolutionDir)libs/" }

	filter("configurations:release")
		-- release configs
		targetname "tester"
		defines {"NDEBUG"}
		debugdir( release_path ) 
		symbols("Off")
		optimize("Off")
		-- link with ftxui library
		links{ libs_path.."/ftxui/ftxui-component.lib" }
		links{ libs_path.."/ftxui/ftxui-dom.lib" }
		links{ libs_path.."/ftxui/ftxui-screen.lib" }
		
	filter("configurations:debug")
		-- debug configs
		targetname "tester"
		defines {"NDEBUG"}
		symbols("On")
		optimize("Off")
		debugdir( debugging_path ) 
		-- link with ftxui library
		links{ libs_path.."/ftxui/d_ftxui-component.lib" }
		links{ libs_path.."/ftxui/d_ftxui-dom.lib" }
		links{ libs_path.."/ftxui/d_ftxui-screen.lib" }
	
-- ==============================
-- ==============================
   