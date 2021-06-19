cmake_minimum_required(VERSION 3.19)

project(parable-engine VERSION 0.0 DESCRIPTION "Game engine" LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# apparently this helps VSCode to recognise cmake includes
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

add_subdirectory(Parable)

add_subdirectory(Testapp)