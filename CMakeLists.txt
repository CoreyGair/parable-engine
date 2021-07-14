cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(parable-engine VERSION 0.0 DESCRIPTION "Game engine" LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# apparently this helps VSCode to recognise cmake includes
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

# add thread libs
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")

add_subdirectory(Parable)

add_subdirectory(Testapp)

# add tests
add_subdirectory(tests)