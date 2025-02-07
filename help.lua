
function print_error(message)
    print('\27[31m'..message..'\27[0m')
end

function print_hint(message)
    print('\27[32m'..message..'\27[0m')
end

function print_warn(message)
      print('\27[33m'..message..'\27[0m')
end

function _to_windows_path(_path)
    local new_path = ""

    for i = 1, #_path do
        local c = _path:sub(i,i)
        if(c=='/') then new_path = new_path .. '\\'
        else new_path = new_path .. c
        end
    end

    return new_path
end

function _copy(src , dest)
    local _ok , _err 
    PATH = os.getcwd().."/"

    if(os.target() == "windows") then
        local wsrc  = _to_windows_path(PATH .. src)
        local wdest = _to_windows_path(PATH .. dest)

        _ok , _err = os.execute("xcopy /Y /f " .. wsrc .. " " .. wdest)

        if(_ok==nil) then 
            print("ERROR:: " , _err)
            return 0
        else 
            return 1
        end

    end

    if(os.target() == "linux") then
    
        _ok , _err = os.execute("cp " .. src .. dest)

        if(_ok==nil) then 
            print("ERROR:: " , _err)
            return 0
        else 
            return 1
        end

    end
    
    return 1
end

function check_files(files) 
    all_in = 1
    PATH = os.getcwd().."/"

    for i = 1, #files do
        if( os.isfile(PATH .. files[i]) == false ) then 
            all_in = 0
            break 
        end
    end

    return all_in
end

function remove_temp_folder()

    if(os.target() == "windows") then
        local ok , err = os.execute("rd /s /q temp")
        return { ok , err }
    end

    if(os.target() == "linux") then
        local ok , err = os.execute("rm -rf temp")
        return { ok , err }
    end

    return { 0 , 0 }
end