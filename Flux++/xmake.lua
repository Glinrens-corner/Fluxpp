set_config("toolchain", "clang")

add_repositories("private-repo private_repo")
add_requires("doctest")
add_requires("mem_comparable_closure")

target("test")
   set_kind("binary")
   add_files("src/*.cpp")
   add_files("test/*.cpp")
   if ( get_config("toolchain") ~= "clang" ) then
      print("only set up for clang")
       set_enabled(false)
   end
   add_packages("mem_comparable_closure")
   add_packages("doctest")
   set_languages("cxx17")
   add_includedirs("include")
   set_optimize("fastest")
   set_warnings("allextra")
   
