set(PARABLE_SRCS_CORE       ${CMAKE_CURRENT_SOURCE_DIR}/Core/Application.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Core/Log.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Core/LayerStack.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Core/Time.cpp
                            )

set(PARABLE_SRCS_EVENTS     ${CMAKE_CURRENT_SOURCE_DIR}/Events/EventBuffer.cpp
                            )

set(PARABLE_SRCS_PLATFORM   ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/VulkanInstance.cpp
                            )

set(PARABLE_SRCS_WINDOW     ${CMAKE_CURRENT_SOURCE_DIR}/Window/Window.cpp
                            )

set(PARABLE_SRCS_DEBUG      ${CMAKE_CURRENT_SOURCE_DIR}/Debug/EventLogLayer.cpp
                            )

set(PARABLE_SRCS_INPUT      ${CMAKE_CURRENT_SOURCE_DIR}/Input/InputLayer.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Input/InputManager.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Input/InputContext.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Input/ButtonMap.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Input/InputContextLoader.cpp
                            )

set(PARABLE_SRCS_MEMORY     ${CMAKE_CURRENT_SOURCE_DIR}/Memory/LinearAllocator.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Memory/PoolAllocator.cpp
                            ) 

set(PARABLE_SRCS_UTIL   ${CMAKE_CURRENT_SOURCE_DIR}/Util/DynamicBitset.cpp
                        )

set(PARABLE_SRCS_ECS    ${CMAKE_CURRENT_SOURCE_DIR}/ECS/EntityManager.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/ECS/ComponentManager.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/ECS/SystemManager.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/ECS/EntityComponentMap.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/ECS/ECS.cpp
                        ) 

set(PARABLE_SRCS_EXCEPTION      
                                )

set(PARABLE_SRCS    ${PARABLE_SRCS_CORE}
                    ${PARABLE_SRCS_EVENTS}
                    ${PARABLE_SRCS_PLATFORM}
                    ${PARABLE_SRCS_WINDOW}
                    ${PARABLE_SRCS_DEBUG}
                    ${PARABLE_SRCS_INPUT}
                    ${PARABLE_SRCS_MEMORY}
                    ${PARABLE_SRCS_UTIL}
                    ${PARABLE_SRCS_ECS}
                    ${PARABLE_SRCS_EXCEPTION}
                    PARENT_SCOPE)