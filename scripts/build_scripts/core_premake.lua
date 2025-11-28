
-----------------------------------------------------------------------
-- this script file here to describe core library project --
-----------------------------------------------------------------------
utility = require("utility_functions")

local core = {
    project_name = "core",
    name = "core",
    kind = "StaticLib",
    arch = "x64",
    lang = "C++",
    lang_version = "C++17",
	
	lib = {
		filename = "core.lib",
		release_path = utility.s_paths.release .. "/core/",
		debug_path  = utility.s_paths.debug .. "/core/",
	},
	
	dll = {
		filename      = "core_d.dll",
		lib_filename = "core_d.lib",
		release_path = utility.s_paths.release .. "/core/",
		debug_path  = utility.s_paths.debug .. "/core/",
	},
	
}

if _OS == "windows" then
	core.dll.release_post_script = "copy " .. path.translate(core.dll.release_path  .. core.dll.filename .. " " .. utility.s_paths.build) 
	core.dll.debug_post_script  = "copy " .. path.translate(core.dll.debug_path  .. core.dll.filename .. " " .. utility.s_paths.build)
else 
	core.dll.release_post_script = "copy " .. (core.dll.release_path  .. core.dll.filename .. " " .. utility.s_paths.build) 
	core.dll.debug_post_script  = "copy " .. (core.dll.debug_path  .. core.dll.filename .. " " .. utility.s_paths.build)
end

local link_with = {
    spdlog_debug  = utility.s_paths.libs .. "/spdlog/spdlogd.lib" ,
    spdlog_release = utility.s_paths.libs .. "/spdlog/spdlog.lib" 
}

-- function run( )

	-- start generate engine project solution
	print('\27[34m' .. "==================================" .. '\27[0m')
	print('\27[34m' .. "GENERATE --> CORE LIBRARY PROJECT" .. '\27[0m')

	project(core.project_name)
	location(paths.ide_projects_dir)

	-- project config
	architecture(core.arch)
	language(core.lang)
	cppdialect(core.lang_version)
	
	configurations({ 
			"debug" , 
			"release" , 
			"dll debug" , 
			"dll release" ,
	})

	
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
	targetdir(core.lib.release_path)
	objdir(core.lib.release_path)

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
filter("configurations:dll release")
	kind("SharedLib")
	
	characterset("Unicode")
	buildoptions({"/utf-8"}) -- for fmt library

	-- build output path
	targetdir(core.dll.release_path)
	objdir(core.dll.release_path)

	-- libraries core need to link with
	links( link_with.spdlog_release )

	targetname(core.name .. "_d")
	
	-- post script to copy dll to build folder 
	postbuildcommands({
		core.dll.release_post_script,
		"echo 'copying core dll release to build folder' "
	})
	
	-- few macros for release
	defines({"NDEBUG", "NO_DEBUG" , "DLL_EXPORT"})

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
	targetdir(core.lib.debug_path)
	objdir(core.lib.debug_path)

	targetname(core.name)

	-- libraries core need to link with
	links( link_with.spdlog_debug )

	debugdir(core.lib.debug_path)
	defines("DEBUG")

	symbols("On")
	optimize("Off")
--------------------------------------------
--------------------------------------------

-----------------------------------------------
-- core project --> DLL debug config
-----------------------------------------------
filter("configurations:dll debug")
	kind("SharedLib")

	characterset("Unicode")
	buildoptions({"/utf-8"}) -- for fmt library

	-- build output path
	targetdir(core.dll.debug_path)
	objdir(core.dll.debug_path)

	targetname(core.name .. "_d")

	-- libraries core need to link with
	links( link_with.spdlog_debug )

	-- post script to copy dll to build folder 
	postbuildcommands({
		core.dll.debug_post_script,
		"echo 'copying core dll debug to build folder' "
	})
	
	debugdir(utility.s_paths.build)
	defines({"DEBUG", "DLL_EXPORT"})

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
	core_project = core
}