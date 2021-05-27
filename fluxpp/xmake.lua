



add_requires("doctest")

target("fluxpp_components")
    set_kind("static")
    set_languages("cxx17")
    add_files("src/*.cpp")
    add_includedirs("include")
    set_optimize("debug")
    set_symbols("debug")
    set_warnings("allextra")
    add_packages("transparent_closure")

target("fluxpp_test")
    set_kind("binary")
    set_default(false)
    set_languages("cxx17")
    add_deps("fluxpp_components")
    add_files("test/*.cpp")
    add_includedirs("include")
    add_packages("doctest")
    set_optimize("debug")
    set_symbols("debug")
