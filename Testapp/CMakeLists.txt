set(TESTAPP_SRCS    ${CMAKE_CURRENT_SOURCE_DIR}/src/Testapp.cpp
                    )

add_executable(Testapp ${TESTAPP_SRCS})

target_link_libraries(Testapp Parable)