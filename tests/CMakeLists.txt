set(TEST_INPUT_SYSTEM   ${CMAKE_CURRENT_SOURCE_DIR}/test_input_system/test_button_map.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/test_input_system/test_button_map.h
                        )

set(TEST_MEMORY     ${CMAKE_CURRENT_SOURCE_DIR}/test_memory/test_allocators.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/test_memory/test_allocators.h
                    )

set(TEST_UTIL       ${CMAKE_CURRENT_SOURCE_DIR}/test_util/test_bitset.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/test_util/test_bitset.h
                    )

# GTEST
# update the hash to the latest git commit hash for gtest to update
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/96f4ce02a3a78d63981c67acbd368945d11d7d70.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable( parable-core-test
                test_main.cpp
                ${TEST_INPUT_SYSTEM}
                ${TEST_MEMORY}
                ${TEST_UTIL}
                )
target_include_directories(parable-core-test PUBLIC include)
target_link_libraries(parable-core-test Parable gtest)



include(GoogleTest)
gtest_discover_tests(parable-core-test)