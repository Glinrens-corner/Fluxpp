package("mem_comparable_closure")
    set_description("for closures that can be compared")
    set_urls("https://github.com/Glinrens-corner/mem_comparable_closure.git")
    add_versions("0.0.1", "4f0a6c9cf4a51835ab5213afac9eee63cc4ca339")
    add_versions("0.0.2", "554335b6a76d25cad91cffbb7a247e89e58e97d4") 
    on_install(function(package)
        os.cp("mem_comparable_closure/include/*", package:installdir("include"))
    end)
