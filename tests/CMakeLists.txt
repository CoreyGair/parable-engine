set(TEST_INPUT_SYSTEM   ${CMAKE_CURRENT_SOURCE_DIR}/test_input_system/test_button_map.cpp
                        )

set(TEST_MEMORY     ${CMAKE_CURRENT_SOURCE_DIR}/test_memory/test_allocators.cpp
                    )

set(TEST_UTIL       ${CMAKE_CURRENT_SOURCE_DIR}/test_util/test_bitset.cpp
                    )

add_executable(parable_core_tests   ${TEST_INPUT_SYSTEM}
                                    ${TEST_MEMORY}
                                    ${TEST_UTIL}
                                    )

target_link_libraries(parable_core_tests PUBLIC Parable Catch2WithMain)