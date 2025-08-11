package("behaviortree")

    set_homepage("https://github.com/BehaviorTree/BehaviorTree.CPP")

    add_urls("https://github.com/BehaviorTree/BehaviorTree.CPP/archive/refs/tags/$(version).tar.gz",
             "https://github.com/BehaviorTree/BehaviorTree.CPP.git")
    add_versions("4.7.0", "ee71a20daa245b4a8eb27c8352b0cb144831c456bdac4ed894694a1f78e407da")

    add_deps("cmake")
    -- add_deps("sqlite3")
    -- add_deps("zeromq")
    -- well, gencode path is hardcode
    -- add_includedirs("include/antlr4-runtime")

    on_install("macosx", "linux", "windows", "mingw", "cross", function (package)

        local configs = {"-DCMAKE_CXX_STANDARD=23"}
        table.insert(configs, "-DBTCPP_EXAMPLES=OFF") -- 关闭样例
        table.insert(configs, "-DBUILD_TESTING=OFF") -- 关闭单测
        table.insert(configs, "-DBTCPP_GROOT_INTERFACE=OFF") 
        table.insert(configs, "-DBTCPP_SQLITE_LOGGING=OFF") 
        table.insert(configs, "-DBTCPP_BUILD_TOOLS=OFF") 
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        if package:config("shared") then 
            table.insert(configs, "-DBTCPP_SHARED_LIBS=ON")
        else 
            table.insert(configs, "-DBTCPP_SHARED_LIBS=OFF")
            table.insert(configs, "-DFORCE_STATIC_LINKING=ON")
        end
        -- os.cd(path.join(os.curdir(), "runtime","Cpp"))
        import("package.tools.cmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            #include "behaviortree_cpp/bt_factory.h"
            int main()
            {
            using namespace BT;
            BehaviorTreeFactory factory;
            }
        ]]}, {configs = {languages = "cxx23"}}))
    end)