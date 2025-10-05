
-----------------------------------------------------------------------
-- this script file here to describe core library project --
-----------------------------------------------------------------------
utility = require("utility_functions")

local core = {
    project_name = "core",
    name = "core",
	dll_name = "core_d",
    kind = "StaticLib",
    arch = "x64",
    lang = "C++",
    lang_version = "C++17"
}

local link_with = {
    spdlog_debug  = utility.s_paths.libs .. "/spdlog/spdlogd.lib" ,
    spdlog_release = utility.s_paths.libs .. "/spdlog/spdlog.lib" 
}

-- function run( )

	-- start generate engine project solution
	print('\27[34m' .. "==================================" .. '\27[0m')
	print('\27[34m' .. "GENERATE CORE LIB PROJECT" .. '\27[0m')

	project(core.project_name)
	location(paths.ide_projects_dir)

	-- project config
	architecture(core.arch)
	language(core.lang)
	cppdialect(core.lang_version)
	configurations({ "debug", "dll_debug", "release", "dll_release" })
	
	files { 
			"../../core/**" 
	}
	includedirs(
		{ 
			utility.s_paths.root,
			utility.s_paths.libs .. "boxer",
			utility.s_paths.libs
		}
	)

------------------------------------------------
-- core project --> static release config
------------------------------------------------
filter("configurations:release")
	kind("StaticLib")
	
	characterset("Unicode")
	buildoptions({"/utf-8"}) -- for fmt library

	-- build output path
	targetdir(utility.s_paths.release .. "/core/")
	objdir(utility.s_paths.release .. "/core/")

	-- libraries core need to link with
	links( link_with.spdlog_release )

	targetname(core.name)

	-- few macros for release
	defines({"NDEBUG", "NO_DEBUG"})

	symbols("Off")
	optimize("Off")
	--------------------------------------------
	--------------------------------------------
	
----------------------------------------------
-- core project --> DLL release config
----------------------------------------------
filter("configurations:dll_release")
	kind("SharedLib")
	
	characterset("Unicode")
	buildoptions({"/utf-8"}) -- for fmt library

	-- build output path
	targetdir(utility.s_paths.build)
	objdir(utility.s_paths.release .. "/core/")

	-- libraries core need to link with
	links( link_with.spdlog_release )

	targetname(core.dll_name)

	-- few macros for release
	defines({"NDEBUG", "NO_DEBUG" , "CORE_DLL"})

	symbols("Off")
	optimize("Off")
	--------------------------------------------
	--------------------------------------------

-----------------------------------------------
-- core project --> static debug config
-----------------------------------------------
filter("configurations:debug")
	kind("StaticLib")

	characterset("Unicode")
	buildoptions({"/utf-8"}) -- for fmt library

	-- build output path
	targetdir(utility.s_paths.debug .. "/core/")
	objdir(utility.s_paths.debug .. "/core/")

	targetname(core.name)

	-- libraries core need to link with
	links( link_with.spdlog_debug )

	debugdir(utility.s_paths.build)
	defines("DEBUG")

	symbols("On")
	optimize("Off")
--------------------------------------------
--------------------------------------------

-----------------------------------------------
-- core project --> DLL debug config
-----------------------------------------------
filter("configurations:dll_debug")
	kind("SharedLib")

	characterset("Unicode")
	buildoptions({"/utf-8"}) -- for fmt library

	-- build output path
	targetdir(utility.s_paths.build)
	objdir(utility.s_paths.debug .. "/core/")

	targetname(core.dll_name)

	-- libraries core need to link with
	links( link_with.spdlog_debug )

	debugdir(utility.s_paths.build)
	defines({"DEBUG", "CORE_DLL"})

	symbols("On")
	optimize("Off")
--------------------------------------------
--------------------------------------------

	print("project : " .. core.project_name)
	print("architecture : " .. core.arch)
	print("kind  : " .. core.kind)
	print("language  : " .. core.lang_version)
	print('\27[34m' .. "==================================" .. '\27[0m')

-- end -- function run

return {
	-- run = run,
	core_project = core
}