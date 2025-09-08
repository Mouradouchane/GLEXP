active_debug = true

function DEBUG_LINE(info)
    if active_debug then
        print("line : " .. tostring(info.currentline))
    end
end

s_paths = {
    root = "$(SolutionDir)",
    build = "$(SolutionDir)build",
    release = "$(SolutionDir)build",
    debug = "$(SolutionDir)build",
    common = "$(SolutionDir)common",
    engine = "$(SolutionDir)engine",
    sandbox = "$(SolutionDir)sandbox",
    libs = "$(SolutionDir)libs",
    tools = "$(SolutionDir)tools",
    tester = "$(SolutionDir)tools/tester",
    assimp_dll = "$(SolutionDir)libs/assimp/assimp-vc143-mt.lib",
}

paths = {
    solution_dir = "$(SolutionDir)",
    root = "../../",
    build = "../../build",
    release = "../../build",
    debug = "../../build",
    common = "../../common",
    engine = "../../engine",
    sandbox = "../../sandbox",
    libs = "../../libs",
    tools = "../../tools",
    tester = "../../tools/tester",
}

return {
    paths = paths,
    s_paths = s_paths,
    DEBUG_LINE = DEBUG_LINE
}
