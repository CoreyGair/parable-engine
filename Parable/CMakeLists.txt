

set(PARABLE_SRCS_CORE   ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Application.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Application.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/EntryPoint.h
                        )

set(PARABLE_SRCS    ${PARABLE_SRCS_CORE}
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/Parable.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/pblpch.h
                    )

add_library(Parable STATIC ${PARABLE_SRCS})

target_include_directories(Parable PUBLIC src)

target_precompile_headers(Parable PRIVATE src/pblpch.h)
