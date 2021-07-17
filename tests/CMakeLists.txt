set(TEST_INPUT_SYSTEM   ${CMAKE_CURRENT_SOURCE_DIR}/test_input_system/test_button_map.cpp
                        )

add_executable(parable_core_tests   ${TEST_INPUT_SYSTEM}
                                    )

target_link_libraries(parable_core_tests PUBLIC Parable Catch2WithMain)