
------------------------------------------------------------
-- this script file here to describe engine prject  --
------------------------------------------------------------

utility = require("utility_functions")


local engine = {
    project_name = "engine",
    name           =  "glexp",
    kind             = "WindowedApp",
    arch             = "x64",
    lang             = "C++",
    lang_version = "C++17",
}

local link_with = {
    core_debug        = utility.s_paths.debug   .. "/core/core.lib" ,
    core_release       = utility.s_paths.release .. "/core/core.lib",
	core_dll_debug   = utility.s_paths.build  .. "/core_d.lib" ,
	core_dll_release  = utility.s_paths.build  .. "/core_d.lib" ,
}

-- function run( )
	 
	print('\27[34m' .. "==================================" .. '\27[0m')
	print('\27[34m' .. "GENERATE ENGINE PROJECT" .. '\27[0m')

	-- start generate engine project solution

	-- project name
	engine_project = project( engine.project_name )
	location(paths.ide_projects_dir)
	architecture(engine.arch)

	-- project type
	kind( engine.kind )
	language( engine.lang )
	cppdialect( engine.lang_version )

	-- project dependson 
	dependson({core.core_project.project_name})

	-- compiled binaries output folder
	bindirs( { utility.s_paths.build .. "/binaries/" } )

	-- include dirs
	includedirs ( 
		{ 
			utility.s_paths.root,
			utility.s_paths.libs .. "boxer",
			utility.s_paths.libs
		} 
	)

	-- project source folders
	files( 
		{
		   "../../engine/**"
		} 
	)

	-- engine exe icon
	filter ( "system:windows" )
	icon( utility.s_paths.engine .. "/icon.ico" )

	--------------------------------------------
	-- engine project --> release config
	--------------------------------------------
	filter("configurations:release")

	characterset("Unicode")
	buildoptions({"/utf-8"}) -- for fmt library

	-- compiled exe and obj output folder
	objdir(utility.s_paths.release .. "/engine/")  -- bin's

	targetdir(utility.s_paths.build) -- exe path
	targetname( engine.name .. "_x64" ) -- exe name

	-- libs we need to link
	links( "opengl32.lib" ) -- opengl32
	links( utility.s_paths.libs .. "/glew/glew32.lib" ) -- glew
	links( utility.s_paths.libs .. "/glfw/glfw3dll.lib" ) -- glfw
	links( utility.s_paths.assimp_dll ) -- assimp
	links( link_with.core_release )

	-- few macros for release
	defines (
		{ "NDEBUG" , "NO_DEBUG" }
	)

	symbols("Off")
	optimize("Off")  -- TODO: enable optimizations later
	--------------------------------------------
	--------------------------------------------

	--------------------------------------------
	-- engine project --> link with dll release config
	--------------------------------------------
	filter("configurations:dll_release")

	characterset("Unicode")
	buildoptions({"/utf-8"}) -- for fmt library

	-- compiled exe and obj output folder
	objdir(utility.s_paths.release .. "/engine/")  -- bin's

	targetdir(utility.s_paths.build) -- exe path
	targetname( engine.name .. "_x64" ) -- exe name

	-- libs we need to link
	links( "opengl32.lib" ) -- opengl32
	links( utility.s_paths.libs .. "/glew/glew32.lib" ) -- glew
	links( utility.s_paths.libs .. "/glfw/glfw3dll.lib" ) -- glfw
	links( utility.s_paths.assimp_dll ) -- assimp
	links( link_with.core_dll_release )

	-- few macros for release
	defines (
		{ "NDEBUG" , "NO_DEBUG" , "DLL_IMPORT"}
	)

	symbols("Off")
	optimize("Off")  -- TODO: enable optimizations later
	--------------------------------------------
	--------------------------------------------

	--------------------------------------------
	-- engine project --> debug config
	--------------------------------------------
	filter("configurations:debug")

	characterset("Unicode")
	buildoptions({"/utf-8"}) -- for fmt library

	-- compiled exe and obj output folder
	objdir(utility.s_paths.debug  .. "/engine/")  -- bin's

	targetdir(utility.s_paths.build) -- exe path
	targetname( engine.name .. "_dx64") -- exe name 

	debugdir(utility.paths.build) 

	-- libs we need to link
	links( "opengl32.lib" ) -- opengl32
	links( utility.s_paths.libs .. "/glew/glew32s.lib" ) -- glew
	links( utility.s_paths.libs .. "/glfw/glfw3.lib" ) -- glfw
	links( utility.s_paths.assimp_dll ) -- assimp
	links( link_with.core_debug )

	-- few macros for debug 
	defines ({ "DEBUG" , "GLEW_STATIC" }) -- for glew static linking

	symbols("On")
	optimize("Off")
	----------------------------------------------------
	----------------------------------------------------

	--------------------------------------------
	-- engine project --> link with dlls debug config
	--------------------------------------------
	filter("configurations:dll_debug")

	characterset("Unicode")
	buildoptions({"/utf-8"}) -- for fmt library

	-- compiled exe and obj output folder
	objdir(utility.s_paths.debug  .. "/engine/")  -- bin's

	targetdir(utility.s_paths.build) -- exe path
	targetname( engine.name .. "_dx64") -- exe name 

	debugdir(utility.paths.build) 

	-- libs we need to link
	links( "opengl32.lib" ) -- opengl32
	links( utility.s_paths.libs .. "/glew/glew32s.lib" ) -- glew
	links( utility.s_paths.libs .. "/glfw/glfw3.lib" ) -- glfw
	links( utility.s_paths.assimp_dll ) -- assimp
	links( link_with.core_dll_debug )

	-- few macros for debug
	defines ({ "DEBUG" , "GLEW_STATIC" , "DLL_IMPORT"}) -- for glew static linking

	symbols("On")
	optimize("Off")
	----------------------------------------------------
	----------------------------------------------------

	-- TODO : this need more work + need to be good 
	-- disable few warning related to libs
	-- disablewarnings( "C26812" ) -- assimp 
	-- disablewarnings( "C26451" ) -- assimp

	print("project : " .. engine.project_name)
	print("architecture : " .. engine.arch)
	print("kind  : " .. engine.kind)
	print("language  : " .. engine.lang_version)
	print('\27[34m' .. "==================================" .. '\27[0m')

-- end -- function run 

return {
	-- run = run,
	engine_project = engine
}