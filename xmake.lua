add_rules("mode.debug", "mode.release")

target("yatha")
    set_kind("binary")
    set_languages("c++17")

    add_files("src/*.cpp")

    add_includedirs("include")
    add_includedirs("third_party/cppjieba")

    set_targetdir("bin")
    set_rundir("$(projectdir)")
    set_runargs("input1.txt", "output.txt")
    
    if is_plat("windows") then 
        if has_config("cc", "cl") then
            add_cxflags("/utf-8") 
        else
            add_cxflags("-finput-charset=UTF-8", "-fexec-charset=UTF-8")
        end
    elseif is_plat("linux", "macosx") then 
        add_syslinks("pthread", "m")
    end

