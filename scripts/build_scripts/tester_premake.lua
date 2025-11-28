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

-- function run( )

	-- start generate engine project solution
	print("")
	print('\27[34m' .. "==================================" .. '\27[0m')
	print('\27[34m' .. "GENERATE --> TESTER PROJECT" .. '\27[0m')
	print('\27[34m' .. "==================================" .. '\27[0m')

	project(tester.project_name)
	location(paths.ide_projects_dir)

	-- project config
	kind(tester.kind)
	architecture(tester.arch)
	language(tester.lang)
	cppdialect(tester.lang_version)
	
	configurations({ 
			"debug" , 
			"release" , 
			"dll debug" , 
			"dll release" ,
	})
	
	-- project dependson 
	dependson({core.core_project.project_name})
	
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

-----------------------------------------------------------
-- tester project --> release with .lib's config
-----------------------------------------------------------
	filter("configurations:release")

		-- build output path
		targetdir(utility.s_paths.build .. "/testers/")
		objdir(utility.s_paths.release .. "/tester/")

		targetname(tester.name)
		links( core.core_project.lib.release_path .. core.core_project.lib.filename )

		-- few macros for release
		defines({"NDEBUG", "NO_DEBUG"})

		symbols("Off")
		optimize("Off")
----------------------------------------------------------
----------------------------------------------------------

----------------------------------------------------------
-- tester project --> release with .DLL's config
----------------------------------------------------------
	filter("configurations:dll release")

		-- build output path
		targetdir(utility.s_paths.build .. "/testers/")
		objdir(utility.s_paths.release .. "/tester/")

		targetname(tester.name)
		links( core.core_project.dll.release_path .. core.core_project.dll.lib_filename )

		-- few macros for release
		defines({"NDEBUG", "NO_DEBUG" , "DLL_IMPORT"})

		symbols("Off")
		optimize("Off")
	---------------------------------------------------------
	---------------------------------------------------------
	
	--------------------------------------------
	-- tester project --> debug_with_libs config
	--------------------------------------------
	filter("configurations:debug")

	-- build output path
	targetdir(utility.s_paths.build .. "/testers/")
	objdir(utility.s_paths.debug .. "/tester/")

	targetname(tester.name)
	links( core.core_project.lib.debug_path .. core.core_project.lib.filename )

	defines({"DEBUG"})
	debugdir(utility.s_paths.build .. "/testers/")
	
	symbols("On")
	optimize("Off")
	--------------------------------------------
	--------------------------------------------

	--------------------------------------------
	-- tester project --> debug_with_dlls config
	--------------------------------------------
	filter("configurations:dll debug")

	-- build output path
	targetdir(utility.s_paths.build .. "/testers/")
	objdir(utility.s_paths.debug .. "/tester/")

	targetname(tester.name)
	links( core.core_project.dll.debug_path .. core.core_project.dll.lib_filename )

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