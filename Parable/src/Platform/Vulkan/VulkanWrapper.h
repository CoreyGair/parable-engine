#pragma once

/**
 * @file VulkanWrapper.h
 * @brief Groups together vulkan wrapper classes.
 * 
 * All wrapper classes carry the same null by default semantics of the vk:: (vulkan.hpp) handle classes.
 * That is, the default constructor leaves them uninitialised, with init through defined constructors or move ctors.
 */


#include "VulkanBase.h"


// TODO: check .cpp files, some are unused (impl moved to header, may be worth doing for all?)
// TODO: enforce consistent style over these wrappers
//          - should they all be header-only
//          - consistent styles (all have same vk:: type conversions, ctors, calling conventions, etc)

#include "Platform/Vulkan/Device.h"
#include "Platform/Vulkan/PhysicalDevice.h"
#include "Platform/Vulkan/Swapchain.h"
#include "Platform/Vulkan/Renderpass.h"
#include "Platform/Vulkan/ShaderLoader.h"
#include "Platform/Vulkan/GraphicsPipeline.h"
#include "Platform/Vulkan/Framebuffers.h"
#include "Platform/Vulkan/CommandPool.h"
#include "Platform/Vulkan/Buffer.h"
#include "Platform/Vulkan/Image.h"
#include "Platform/Vulkan/DescriptorSets.h"
