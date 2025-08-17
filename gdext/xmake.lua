add_rules("mode.debug", "mode.release")

set_languages("c++23")

add_requires("godotcpp4 4.4")
add_repositories("my-repo 3rd")
add_requires("behaviortree")

includes("gdexample")
includes("gdbt")

