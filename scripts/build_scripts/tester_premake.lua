------------------------------------------------------------
-- this script file here to describe tester project --
------------------------------------------------------------
utility = require("utility_functions")

local tool = {
    project_name = "tool_tester",
    name = "tester",
    kind = "ConsoleApp",
    arch = "x64",
    lang = "C++",
    lang_version = "C++17"
}

local link_with = {
    common = "common_lib",
}

-- start generate engine project solution
print('\27[34m' .. "==================================" .. '\27[0m')
print('\27[34m' .. "GENERATE TESTER PROJECT" .. '\27[0m')

location("../../")
project(tool.project_name)

-- project config
kind(tool.kind)
architecture(tool.arch)
language(tool.lang)
cppdialect(tool.lang_version)

-- build output path
targetdir(utility.s_paths.build)
    objdir(utility.s_paths.build .. "/binaries/tester/")

-- project files and directories 
files(
    { 
        "../../tools/tester/**" , 
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

targetname(tool.name)
links( link_with.common )

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

targetname(tool.name)
links( link_with.common )

defines("DEBUG")
debugdir(utility.s_paths.debug)

symbols("On")
optimize("Off")
--------------------------------------------
--------------------------------------------

print("project : " .. tool.project_name)
print("architecture : " .. tool.arch)
print("kind  : " .. tool.kind)
print("language  : " .. tool.lang_version)
print('\27[34m' .. "==================================" .. '\27[0m')
