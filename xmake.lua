set_project("inifile")
set_version("0.0.1")

add_rules("mode.debug", "mode.release")

add_requires("gtest", {configs = {main = true}})
add_requires("fmt")

target("stralgo", function()
    set_kind("object")
    add_files("src/stralgo.cpp")
end)

target("inifile", function()
    set_kind("static")
    add_files("src/inifile.cpp")
    add_includedirs("include", {public = true})
    add_packages("fmt", {public = true})
    add_deps("stralgo")
end)

-- A simple interactive demo.
target("interactive", function()
    set_kind("binary")
    set_default(false)
    add_deps("inifile")
    add_files("example/interactive.cpp")
end)

target("test.unit.stralgo", function()
    set_kind("binary")
    set_default(false)

    set_group("test.unit")
    add_packages("gtest")

    add_files("src/stralgo.test.cpp")
    add_deps("stralgo")
end)

target("test.system.basic_decode_encode", function()
    set_kind("binary")
    set_default(false)

    set_group("test.system")
    add_packages("gtest")

    add_files("test/basic_decode_encode.cpp")
    add_deps("inifile")
end)
