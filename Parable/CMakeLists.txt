set(PARABLE_SRCS_CORE   ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Application.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Application.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/EntryPoint.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Log.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Log.cpp
                        )

set(PARABLE_SRCS_EVENTS     ${CMAKE_CURRENT_SOURCE_DIR}/src/Events/Event.h
                            )

set(PARABLE_SRCS    ${PARABLE_SRCS_CORE}
                    ${PARABLE_SRCS_EVENTS}
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/Parable.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/pblpch.h
                    )


# Adding libraries
set(PARABLE_VENDOR  ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

add_subdirectory(${PARABLE_VENDOR}/spdlog)
add_subdirectory(${PARABLE_VENDOR}/glfw)
add_subdirectory(${PARABLE_VENDOR}/glad)

add_library(Parable STATIC ${PARABLE_SRCS})

target_link_libraries(Parable spdlog glfw glad)

target_include_directories(Parable PUBLIC src PRIVATE ${PARABLE_VENDOR}/spdlog/include)

target_precompile_headers(Parable PRIVATE src/pblpch.h)
