
----------------------------------------------------------------
-- this script here to describe the entire workspace --
----------------------------------------------------------------

utility = require("utility_functions")

-- start generate ide solution
print('\27[34m' .. "==================================" .. '\27[0m')
print('\27[34m' .. "    GENERATE PROJECT WORKSPACE" .. '\27[0m')
print('\27[34m' .. "==================================" .. '\27[0m')


solution = workspace("glexp_workspace") 
location("../../")

configurations(
    { "debug" , "release" , "dll_debug" , "dll_release" } 
)
