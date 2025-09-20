
-----------------------------------------------------------------------
-- this script file here to describe core library project --
-----------------------------------------------------------------------
utility = require("utility_functions")

local tool = {
    project_name = "core",
    name = "core",
    kind = "StaticLib",
    arch = "x64",
    lang = "C++",
    lang_version = "C++17"
}

local link_with = {
    spdlog_debug  = utility.s_paths.libs .. "/spdlog/spdlogd.lib" ,
    spdlog_release = utility.s_paths.libs .. "/spdlog/spdlog.lib" 
}

-- start generate engine project solution
print('\27[34m' .. "==================================" .. '\27[0m')
print('\27[34m' .. "GENERATE CORE LIB PROJECT" .. '\27[0m')

project(tool.project_name)
location(paths.ide_projects_dir)

-- project config
kind(tool.kind)
architecture(tool.arch)
language(tool.lang)
cppdialect(tool.lang_version)

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

--------------------------------------------
-- core project --> release config
--------------------------------------------
filter("configurations:release")

-- build output path
targetdir(utility.s_paths.release .. "/core/")
objdir(utility.s_paths.release .. "/core/")

-- libraries core need to link with
links( link_with.spdlog_release )

targetname(tool.name)

-- few macros for release
defines({"NDEBUG", "NO_DEBUG"})

symbols("Off")
optimize("Off")
--------------------------------------------
--------------------------------------------

--------------------------------------------
-- core project --> debug config
--------------------------------------------
filter("configurations:debug")

-- build output path
targetdir(utility.s_paths.debug .. "/core/")
objdir(utility.s_paths.debug .. "/core/")

targetname(tool.name)

-- libraries core need to link with
links( link_with.spdlog_debug )

debugdir(utility.s_paths.build)
defines("DEBUG")

symbols("On")
optimize("Off")
--------------------------------------------
--------------------------------------------

print("project : " .. tool.project_name)
print("architecture : " .. tool.arch)
print("kind  : " .. tool.kind)
print("language  : " .. tool.lang_version)
print('\27[34m' .. "==================================" .. '\27[0m')
