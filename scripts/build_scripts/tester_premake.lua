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
    core_debug  = utility.s_paths.debug .. "/core/core.lib" ,
    core_release = utility.s_paths.release .. "/core/core.lib" 
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
	-- tester project --> release config
	--------------------------------------------
	filter("configurations:release")

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
	-- tester project --> debug config
	--------------------------------------------
	filter("configurations:debug")

	-- build output path
	targetdir(utility.s_paths.build .. "/testers/")
	objdir(utility.s_paths.debug .. "/tester/")

	targetname(tester.name)
	links( link_with.core_debug )

	defines("DEBUG")
	debugdir(utility.s_paths.debug)

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