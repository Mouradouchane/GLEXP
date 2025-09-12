
-----------------------------------------------------------------------
-- this script file here to describe common library project --
-----------------------------------------------------------------------
utility = require("utility_functions")

local tool = {
    project_name = "common_lib",
    name = "common",
    kind = "StaticLib",
    arch = "x64",
    lang = "C++",
    lang_version = "C++17"
}


-- start generate engine project solution
print('\27[34m' .. "==================================" .. '\27[0m')
print('\27[34m' .. "GENERATE COMMON LIB PROJECT" .. '\27[0m')

project(tool.project_name)
location(paths.ide_projects_dir)

-- project config
kind(tool.kind)
architecture(tool.arch)
language(tool.lang)
cppdialect(tool.lang_version)

-- build output path
targetdir(utility.s_paths.build)
    objdir(utility.s_paths.build .. "/binaries/common/")

files { "../../common/**" }
includedirs(
    { 
        utility.s_paths.root,
    }
)

--------------------------------------------
-- common project --> release config
--------------------------------------------
filter("configurations:release")

targetname(tool.name)

-- few macros for release
defines({"NDEBUG", "NO_DEBUG"})

symbols("Off")
optimize("Off")
--------------------------------------------
--------------------------------------------

--------------------------------------------
-- common project --> debug config
--------------------------------------------
targetname(tool.name)

debugdir(utility.s_paths.debug)
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
