
------------------------------------------------------------
-- this script file here to describe engine prject  --
------------------------------------------------------------

utility = require("utility_functions")

-- start generate engine project solution

local engine = {
    project_name= "glexp_engine",
    name           =  "glexp",
    kind             = "WindowedApp",
    arch             = "x64",
    lang             = "C++",
    lang_version = "C++17",
}

print('\27[34m' .. "==================================" .. '\27[0m')
print('\27[34m' .. "GENERATE ENGINE PROJECT" .. '\27[0m')

-- prject name
project( engine.project_name )
architecture(engine.arch)

-- project type
kind( engine.kind )
language( engine.lang )
cppdialect( engine.lang_version )

	
-- compiled binaries output folder
bindirs( { utility.paths.build .. "/binaries/" } )

-- compiled exe and obj output folder
targetdir(utility.paths.build) 
objdir(utility.paths.build .. "/binaries/") 

-- include dirs
includedirs ( 
    { 
        utility.paths.common,
        utility.paths.engine,
        utility.paths.libs
    } 
)

-- project source folders
files( 
    {
        utility.paths.common,
        utility.paths.engine,
        utility.paths.libs ,
        utility.paths.build .. "/shaders/",
        utility.paths.build .. "/textures/",
        utility.paths.build .. "/models/"
    } 
)

-- engine exe icon
filter ( "system:windows" )
icon( utility.paths.engine .. "/icon.ico" )
files( 
    { utility.paths.engine .. "/common_eng/resource.rc" } 
)

--------------------------------------------
-- engine project --> release config
--------------------------------------------
filter("configurations:release")

-- libs we need to link
links( "opengl32.lib" ) -- opengl32
links( utility.paths.libs .. "/glew/glew32.lib" ) -- glew
links( utility.paths.libs .. "/glfw/glfw3dll.lib" ) -- glfw
links( utility.paths.assimp_dll ) -- assimp

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
links( utility.paths.libs .. "/glew/glew32s.lib" ) -- glew
links( utility.paths.libs .. "/glfw/glfw3.lib" ) -- glfw
links( utility.paths.assimp_dll ) -- assimp

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
