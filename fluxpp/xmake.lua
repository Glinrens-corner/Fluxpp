
add_requires("fmt")
add_requires("range-v3")
add_requires("transparent_closure")


add_requires("doctest")

target("fluxpp_components")
    set_kind("static")
    set_languages("cxx17")
    add_files("src/*.cpp")
    add_includedirs("include")
    set_optimize("debug")
    set_symbols("debug")
    set_warnings("allextra")
    add_packages("transparent_closure",
                 "fmt",
                 "range-v3"
    )

target("fluxpp_test")
    set_kind("binary")
    set_default(false)
    set_languages("cxx17")
    add_deps("fluxpp_components")
    add_files("test/*.cpp")
    add_includedirs("include")
    add_packages("doctest",
                 "transparent_closure",
                 "fmt",
                 "range-v3")
    set_optimize("debug")
    set_symbols("debug")
