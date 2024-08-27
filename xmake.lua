set_project("inifile")
set_version("0.0.1")

add_rules("mode.debug", "mode.release")

target("inifile", function()
    set_kind("static")
    add_files("src/*.cpp")
    add_includedirs("include", {public = true})
end)
