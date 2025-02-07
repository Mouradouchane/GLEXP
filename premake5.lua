
include "help.lua"

-- workspace/solution setup
workspace("glexp_workspace") 
    configurations{ 
        "debug_static" , "debug_dynamic",
        "release_static", "release_dynamic"
    }
    bindirs("$(SolutionDir)build")

architecture("x64")
-- project setup
project("glexp_project")

files {"**.cpp" , "**.hpp" , "**.h" , "**.c" , "**.vert" , "**.frag","**.ini" , "premake5.lua"} 

-- program icon
filter { 'system:windows' }
    icon("./projec/icon.ico")
    files { "./project/common/resource.rc"}
    --vpaths { ['Resources/*'] = { '*.rc', '**.ico' }

-- include dirs
includedirs{ "./libs/" }
includedirs{ "./project/**" }
includedirs{ "./build/shaders/**" }


-- project type
kind("WindowedApp")
language "C++"

targetdir("build/") -- exe output folder
objdir("build/binaries/") -- obj output folder
  

filter("configurations:debug_dynamic")
    -- libs dirs "dynamic linking"
    links{ "opengl32.lib" }
    links{ "$(SolutionDir)libs/glew/glew32.lib" }
    links{ "$(SolutionDir)libs/glfw/glfw3dll.lib" }
    targetname "glexp_dx64"
    defines { "DEBUG" }
    symbols("On")

filter("configurations:debug_static")
    -- libs dirs "static linking"
    links{ "opengl32.lib" }
    links{ "$(SolutionDir)libs/glew/glew32s.lib" }
    links{ "$(SolutionDir)libs/glfw/glfw3.lib" }
    targetname "glexp_dx64"
    defines("GLEW_STATIC") -- for glew static linking
    defines { "DEBUG" }
    symbols("On")

filter("configurations:release_static")
    -- libs dirs "static linking"
    links{ "opengl32.lib" }
    links{ "$(SolutionDir)libs/glew/glew32s.lib" }
    links{ "$(SolutionDir)libs/glfw/glfw3.lib" }
    targetname "glexp_rx64"
    defines("GLEW_STATIC") -- for glew static linking
    defines {"NDEBUG"}
    symbols("Off")
    optimize "Off"

filter("configurations:release_dynamic")
    -- libs dirs "dynamic linking"
    links{ "opengl32.lib" }
    links{ "$(SolutionDir)libs/glew/glew32.lib" }
    links{ "$(SolutionDir)libs/glfw/glfw3dll.lib" }
    targetname "glexp_rx64"
    defines {"NDEBUG"}
    symbols("Off")
    optimize "Off"

-- downloading libs
print("============ LIBS SETUP:"..os.target().." ============")

cwd = os.getcwd().."/"
ok, err = os.mkdir("temp")
copy_result = 0

check_lib_files = check_files({ 
    "temp/stb/stb_image.h" , 
})

if(check_lib_files==0)then
    -- get stb_image
    os.execute("git clone https://github.com/nothings/stb.git temp/stb")
else 
    print_hint("HINT::'stb' library found")
end

copy_result = _copy("temp/stb/stb_image.h" , "libs/")
if copy_result == 0 then 
    print("WARNING:: failed to copy stb_image to libs !")
    print_warn("WARNING:: project not gonna compile correctly !")
end

check_lib_files = check_files({ 
    "temp/mini/src/mini/ini.h" , 
})

if(check_lib_files==0)then
    -- get mINI
    os.execute("git clone https://github.com/metayeti/mINI.git/ temp/mini")
else 
    print_hint("HINT::'mINI' library found")
end

copy_result = _copy("temp/mini/src/mini/ini.h" , "libs/")
if copy_result == 0 then 
    print_warn("WARNING:: failed to copy mINI to libs !")
    print_warn("WARNING:: project not gonna compile correctly !")
end

newoption {
   trigger = "clean",
   value = 1,
   description = "removed downloaded content after build"
}

if(clean==1) then
    clean_up = remove_temp_folder()
    if(clean_up[0] == nil) then
        print_warn("WARNING:: failed to delete temp folder")
        print_error("ERROR:: " .. clean_up[1])
        print_hint("HINT:: delete temp folder manualy !")
    end
end