set_config("toolchain", "clang")
if ( get_config("toolchain") ~= "clang" ) then
  print("only set up for clang")

end

add_requires("doctest")

target("test")
   set_kind("binary")
   add_files("src/*.cpp")
   add_files("test/*.cpp")
   if ( get_config("toolchain") ~= "clang" ) then
       set_enabled(false)
   end
   add_packages("doctest")
   set_languages("cxx17")
   add_includedirs("include")
   set_optimize("fastest")
   set_warnings("everything")
   
