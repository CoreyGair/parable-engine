set(PARABLE_SRCS_CORE       ${CMAKE_CURRENT_SOURCE_DIR}/Core/Application.h
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
                            ${CMAKE_CURRENT_SOURCE_DIR}/Events/WindowEvent.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/Events/InputEvent.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/Events/EventBuffer.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/Events/EventBuffer.cpp
                            )

set(PARABLE_SRCS_PLATFORM   ${CMAKE_CURRENT_SOURCE_DIR}/Platform/PlatformDetection.h
                            )

set(PARABLE_SRCS_WINDOW     ${CMAKE_CURRENT_SOURCE_DIR}/Window/Window.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/Window/Window.cpp
                            )

set(PARABLE_SRCS_DEBUG      ${CMAKE_CURRENT_SOURCE_DIR}/Debug/EventLogLayer.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/Debug/EventLogLayer.cpp
                            )

set(PARABLE_SRCS_INPUT      ${CMAKE_CURRENT_SOURCE_DIR}/Input/InputCodes.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/Input/InputLayer.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/Input/InputLayer.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Input/InputManager.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/Input/InputManager.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Input/InputContext.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/Input/InputContext.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Input/ButtonMap.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/Input/ButtonMap.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Input/InputContextLoader.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/Input/InputContextLoader.cpp
                            )

set(PARABLE_SRCS_MEMORY     ${CMAKE_CURRENT_SOURCE_DIR}/Memory/Allocator.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/Memory/Allocator.tpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Memory/LinearAllocator.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/Memory/LinearAllocator.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Memory/PoolAllocator.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/Memory/PoolAllocator.cpp
                            ) 

set(PARABLE_SRCS    ${PARABLE_SRCS_CORE}
                    ${PARABLE_SRCS_EVENTS}
                    ${PARABLE_SRCS_PLATFORM}
                    ${PARABLE_SRCS_WINDOW}
                    ${PARABLE_SRCS_DEBUG}
                    ${PARABLE_SRCS_INPUT}
                    ${PARABLE_SRCS_MEMORY}
                    ${CMAKE_CURRENT_SOURCE_DIR}/Parable.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/pblpch.h
                    PARENT_SCOPE)