target("gdbt")
    set_kind("shared")
    add_files("src/*.cpp")
    add_packages("godotcpp4")
    add_packages("behaviortree")

target("gdbb")
    set_kind("binary")
    add_files("src/*.cc")
    add_packages("behaviortree")

