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

-- start generate engine project solution
print('\27[34m' .. "==================================" .. '\27[0m')
print('\27[34m' .. "GENERATE TESTER PROJECT" .. '\27[0m')

project(tool.project_name)

-- project config
kind(tool.kind)
architecture(tool.arch)
language(tool.lang)
cppdialect(tool.lang_version)

-- build output path
targetdir(utility.paths.solution_dir .. utility.paths.build)
objdir(utility.paths.solution_dir .. utility.paths.build .. "/binaries/tester/")

-- project files and directories 
files(
    { "tools/tester/**" }
)
includedirs(
    { "$(SolutionDir)tools/tester/" }
    -- utility.paths.solution_dir 
)

--------------------------------------------
-- tester project --> release config
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
-- tester project --> debug config
--------------------------------------------
filter("configurations:debug")

targetname(tool.name)

defines("DEBUG")
debugdir(utility.paths.solution_dir .. utility.paths.debug)

symbols("On")
optimize("Off")
--------------------------------------------
--------------------------------------------

print("project : " .. tool.project_name)
print("architecture : " .. tool.arch)
print("kind  : " .. tool.kind)
print("language  : " .. tool.lang_version)
print('\27[34m' .. "==================================" .. '\27[0m')
