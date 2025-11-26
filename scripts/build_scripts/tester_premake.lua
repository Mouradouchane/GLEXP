------------------------------------------------------------
-- this script file here to describe tester project --
------------------------------------------------------------
utility = require("utility_functions")

local tester = {
    project_name = "tester",
    name = "tester",
    kind = "ConsoleApp",
    arch = "x64",
    lang = "C++",
    lang_version = "C++17"
}

local link_with = {
    core_debug       = utility.s_paths.debug .. "/core/core.lib" ,
    core_dll_debug  = utility.s_paths.debug .. "/core/core_d.lib" ,
    core_release      = utility.s_paths.release .. "/core/core.lib" ,
    core_dll_release = utility.s_paths.release .. "/core/core_d.lib" 
}

-- function run( )

	-- start generate engine project solution
	print('\27[34m' .. "==================================" .. '\27[0m')
	print('\27[34m' .. "GENERATE TESTER PROJECT" .. '\27[0m')


	project(tester.project_name)
	location(paths.ide_projects_dir)

	-- project config
	kind(tester.kind)
	architecture(tester.arch)
	language(tester.lang)
	cppdialect(tester.lang_version)

	configurations({ 
		"debug + link with .LIB's", 
		"release + link with .LIB's",
		"debug + link with .DLL's",
		"release + link with .DLL's",
	})
	
	-- project files and directories 
	files(
		{
			utility.paths.tester .. "/**"
		}
	)
	includedirs(
		{ 
			utility.s_paths.root,
			utility.s_paths.libs .. "/tabulate/",
			utility.s_paths.libs,
		}
	)

	--------------------------------------------
	-- tester project --> release with .lib's config
	--------------------------------------------
	filter("configurations:release + link with .LIB's")

		-- build output path
		targetdir(utility.s_paths.build .. "/testers/")
		objdir(utility.s_paths.release .. "/tester/")

		targetname(tester.name)
		links( link_with.core_release )

		-- few macros for release
		defines({"NDEBUG", "NO_DEBUG"})

		symbols("Off")
		optimize("Off")
	--------------------------------------------
	--------------------------------------------

	--------------------------------------------
	-- tester project --> release with .DLL's config
	--------------------------------------------
	filter("configurations:release + link with .DLL's")

		-- build output path
		targetdir(utility.s_paths.build .. "/testers/")
		objdir(utility.s_paths.release .. "/tester/")

		targetname(tester.name)
		links( link_with.core_dll_release )

		-- few macros for release
		defines({"NDEBUG", "NO_DEBUG" , "DLL_IMPORT"})

		symbols("Off")
		optimize("Off")
	--------------------------------------------
	--------------------------------------------
	
	--------------------------------------------
	-- tester project --> debug + link with .LIB's config
	--------------------------------------------
	filter("configurations:debug + link with .LIB's")

	-- build output path
	targetdir(utility.s_paths.build .. "/testers/")
	objdir(utility.s_paths.debug .. "/tester/")

	targetname(tester.name)
	links( link_with.core_debug )

	defines({"DEBUG"})
	debugdir(utility.s_paths.build .. "/testers/")
	
	symbols("On")
	optimize("Off")
	--------------------------------------------
	--------------------------------------------

	--------------------------------------------
	-- tester project --> debug + link with .DLL's config
	--------------------------------------------
	filter("configurations:debug + link with .DLL's")

	-- build output path
	targetdir(utility.s_paths.build .. "/testers/")
	objdir(utility.s_paths.debug .. "/tester/")

	targetname(tester.name)
	links( link_with.core_dll_debug )

	defines({"DEBUG" , "DLL_IMPORT"})
	debugdir(utility.s_paths.build .. "/testers/")

	symbols("On")
	optimize("Off")
	--------------------------------------------
	--------------------------------------------
	
	print("project : " .. tester.project_name)
	print("architecture : " .. tester.arch)
	print("kind  : " .. tester.kind)
	print("language  : " .. tester.lang_version)
	print('\27[34m' .. "==================================" .. '\27[0m')

-- end -- function run

return {
	-- run = run,
	tester_config = tester
}