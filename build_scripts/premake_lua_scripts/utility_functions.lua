active_debug = true

function DEBUG_LINE(info)
    if active_debug then
        print("line : " .. tostring(info.currentline))
    end
end

paths = {
    solution_dir = "$(SolutionDir)",
    build = "build",
    release = "build",
    debug = "build",
    common = "common",
    engine = "engine",
    sandbox = "sandbox",
    libs = "libs",
    tools = "tools",
    tester = "tools/tester",
    assimp_dll = "libs/assimp/assimp-vc143-mt.lib",
}

return {
    paths = paths,
    DEBUG_LINE = DEBUG_LINE
}
