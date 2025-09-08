
------------------------------------------------------------
-- this script file here to describe engine prject  --
------------------------------------------------------------

utility = require("utility_functions")

local engine = {
    project_name= "engine",
    name           =  "glexp",
    kind             = "WindowedApp",
    arch             = "x64",
    lang             = "C++",
    lang_version = "C++17",
}

local link_with = {
    common = "common_lib",
}

print('\27[34m' .. "==================================" .. '\27[0m')
print('\27[34m' .. "GENERATE ENGINE PROJECT" .. '\27[0m')

-- start generate engine project solution
--location("../../")

-- prject name
engine_project = project( engine.project_name )
architecture(engine.arch)

-- project type
kind( engine.kind )
language( engine.lang )
cppdialect( engine.lang_version )

-- compiled binaries output folder
bindirs( { utility.s_paths.build .. "/binaries/" } )

-- compiled exe and obj output folder
targetdir(utility.s_paths.build) 
    objdir(utility.s_paths.build .. "/binaries/") 

-- include dirs
includedirs ( 
    { 
        utility.s_paths.root
    } 
)

-- project source folders
files( 
    {
       "../../engine/**"
    } 
)

-- engine exe icon
filter ( "system:windows" )
icon( utility.s_paths.engine .. "/icon.ico" )

--------------------------------------------
-- engine project --> release config
--------------------------------------------
filter("configurations:release")

-- libs we need to link
links( "opengl32.lib" ) -- opengl32
links( utility.s_paths.root .. "libs/glew/glew32.lib" ) -- glew
links( utility.s_paths.root .. "libs/glfw/glfw3dll.lib" ) -- glfw
links( utility.s_paths.assimp_dll ) -- assimp
links( link_with.common )

-- release name
targetname( engine.name .. "_rx64" )

-- few macros for release
defines (
    { "NDEBUG" , "NO_DEBUG" }
)

symbols("Off")
optimize("Off")  -- TODO: enable optimizations later
--------------------------------------------
--------------------------------------------

--------------------------------------------
-- engine project --> debug config
--------------------------------------------
filter("configurations:debug")

-- libs we need to link
links( "opengl32.lib" ) -- opengl32
links( utility.s_paths.root .. "/glew/glew32s.lib" ) -- glew
links( utility.s_paths.root .. "/glfw/glfw3.lib" ) -- glfw
links( utility.s_paths.assimp_dll ) -- assimp
links( link_with.common )

-- engine exe name
targetname( engine.name .. "_dx64")
debugdir(utility.paths.debug) 

-- few macros for debug 
defines ({ "DEBUG" , "GLEW_STATIC" }) -- for glew static linking

symbols("On")
optimize("Off")
----------------------------------------------------
----------------------------------------------------

-- TODO : this need more work + need to be good 
-- disable few warning related to libs
-- disablewarnings( "C26812" ) -- assimp 
-- disablewarnings( "C26451" ) -- assimp

print("project : " .. engine.project_name)
print("architecture : " .. engine.arch)
print("kind  : " .. engine.kind)
print("language  : " .. engine.lang_version)
print('\27[34m' .. "==================================" .. '\27[0m')
