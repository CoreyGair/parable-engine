set(PARABLE_SRCS_CORE       ${CMAKE_CURRENT_SOURCE_DIR}/Core/Application.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Core/Log.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Core/LayerStack.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Core/Time.cpp
                            )

set(PARABLE_SRCS_ASSET      ${CMAKE_CURRENT_SOURCE_DIR}/Asset/AssetRegistry.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Asset/AssetLoadInfo.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Asset/EffectLoadInfo.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Asset/MaterialLoadInfo.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Asset/Handle.cpp
                            )
                            
set(PARABLE_SRCS_EVENTS     ${CMAKE_CURRENT_SOURCE_DIR}/Events/EventBuffer.cpp
                            )

set(PARABLE_SRCS_PLATFORM   ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Renderer.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/ResourceStore.cpp

                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Wrapper/Renderpass.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Wrapper/DescriptorSets.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Wrapper/Image.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Wrapper/GraphicsPipeline.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Wrapper/Framebuffers.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Wrapper/Buffer.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Wrapper/BufferSuballocator.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Wrapper/CommandPool.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Wrapper/ShaderLoader.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Wrapper/Device.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Wrapper/PhysicalDevice.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Wrapper/Swapchain.cpp

                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Loader/Loader.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Loader/LoadTask.cpp

                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Mesh/Mesh.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Mesh/MeshStore.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Mesh/MeshData.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Mesh/MeshLoadTask.cpp

                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Texture/Texture.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Texture/TextureStore.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Texture/TextureData.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Texture/TextureLoadTask.cpp

                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Effect/EffectStore.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Effect/EffectData.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Platform/Vulkan/Effect/EffectLoadTask.cpp
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

set(PARABLE_SRCS_RENDER     ${CMAKE_CURRENT_SOURCE_DIR}/Renderer/Renderer.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Renderer/RenderLayer.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/Renderer/PVD.cpp
                            )

set(PARABLE_SRCS    ${PARABLE_SRCS_CORE}
                    ${PARABLE_SRCS_ASSET}
                    ${PARABLE_SRCS_EVENTS}
                    ${PARABLE_SRCS_PLATFORM}
                    ${PARABLE_SRCS_WINDOW}
                    ${PARABLE_SRCS_DEBUG}
                    ${PARABLE_SRCS_INPUT}
                    ${PARABLE_SRCS_MEMORY}
                    ${PARABLE_SRCS_UTIL}
                    ${PARABLE_SRCS_ECS}
                    ${PARABLE_SRCS_EXCEPTION}
                    ${PARABLE_SRCS_RENDER}
                    PARENT_SCOPE)