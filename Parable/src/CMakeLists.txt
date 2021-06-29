set(PARABLE_SRCS_CORE   ${CMAKE_CURRENT_SOURCE_DIR}/Core/Application.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Application.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/Core/EntryPoint.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Base.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Log.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Log.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Layer.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/Core/LayerStack.h
                        ${CMAKE_CURRENT_SOURCE_DIR}/Core/LayerStack.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Assert.h
                        )

set(PARABLE_SRCS_EVENTS     ${CMAKE_CURRENT_SOURCE_DIR}/Events/Event.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/Events/EventBuffer.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/Events/EventBuffer.cpp
                            )

set(PARABLE_SRCS_PLATFORM   ${CMAKE_CURRENT_SOURCE_DIR}/Platform/PlatformDetection.h
                            )

set(PARABLE_SRCS    ${PARABLE_SRCS_CORE}
                    ${PARABLE_SRCS_EVENTS}
                    ${PARABLE_SRCS_PLATFORM}
                    ${CMAKE_CURRENT_SOURCE_DIR}/Parable.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/pblpch.h
                    PARENT_SCOPE)