
-----------------------------------------------------------------------
-- prmake5.lua is the main script who call other scripts 
-- all of them in --> "./build_scripts/premake_lua_scripts" 
-----------------------------------------------------------------------

utility = require("./scripts/build_scripts/utility_functions")
utility.active_debug = false

-- workspace setup
workspace = require("./scripts/build_scripts/workspace_premake")

-- static lib : core porject setup
core = require("./scripts/build_scripts/core_premake")

-- engine project setup
engine = require("./scripts/build_scripts/engine_premake")

-- sandbox project setup

-- tools : tester projet setup
tester = require("./scripts/build_scripts/tester_premake")

-- tools : models_converter projet setup


