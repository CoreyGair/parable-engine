add_subdirectory(Core)
add_subdirectory(Events)

set(PARABLE_SRCS    ${PARABLE_SRCS_CORE}
                    ${PARABLE_SRCS_EVENTS}
                    ${CMAKE_CURRENT_SOURCE_DIR}/Parable.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/pblpch.h
                    PARENT_SCOPE)