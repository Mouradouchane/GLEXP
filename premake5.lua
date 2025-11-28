
-----------------------------------------------------------------------
-- prmake5.lua is the main script who call other scripts 
-- all of them in --> "./build_scripts/premake_lua_scripts" 
-----------------------------------------------------------------------

utility = require("./scripts/build_scripts/utility_functions")
utility.active_debug = false

-- workspace 
workspace = require("./scripts/build_scripts/workspace_premake")

-- core library project
core = require("./scripts/build_scripts/core_premake")

-- engine project 
engine = require("./scripts/build_scripts/engine_premake")

-- sandbox project

-- tools : tester projet 
tester = require("./scripts/build_scripts/tester_premake")

-- tools : models_converter projet

-- startup project
startproject(engine_project.project_name)

