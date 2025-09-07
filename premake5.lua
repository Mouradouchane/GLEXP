
-----------------------------------------------------------------------
-- prmake5.lua is the main script who call other scripts 
-- all of them in --> "./build_scripts/premake_lua_scripts" 
-----------------------------------------------------------------------

location("../..")

utility = require("./build_scripts/premake_lua_scripts/utility_functions")
-- disable debug prints
utility.active_debug = false

-- workspace setup
workspace_setup = require("./build_scripts/premake_lua_scripts/workspace_build_script")

-- engine project setup
-- engine_setup= require("./build_scripts/premake_lua_scripts/engine_build_script")

-- sandbox project setup 


-- tools : tester projet setup
tester_setup = require("./build_scripts/premake_lua_scripts/tester_build_script")

-- tools : models_converter projet setup
