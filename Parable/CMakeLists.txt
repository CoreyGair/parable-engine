set(PARABLE_SRCS_CORE   ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Application.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Application.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/EntryPoint.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Log.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Log.cpp
                        )

set(PARABLE_SRCS    ${PARABLE_SRCS_CORE}
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/Parable.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/pblpch.h
                    )

set(PARABLE_VENDOR  ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

add_subdirectory(${PARABLE_VENDOR}/spdlog)

add_library(Parable STATIC ${PARABLE_SRCS})

target_link_libraries(Parable spdlog)

target_include_directories(Parable PUBLIC src PRIVATE ${PARABLE_VENDOR}/spdlog/include)

target_precompile_headers(Parable PRIVATE src/pblpch.h)