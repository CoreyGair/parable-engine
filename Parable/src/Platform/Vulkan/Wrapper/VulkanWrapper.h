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

#include "Device.h"
#include "PhysicalDevice.h"
#include "Swapchain.h"
#include "Renderpass.h"
#include "ShaderLoader.h"
#include "GraphicsPipeline.h"
#include "Framebuffers.h"
#include "CommandPool.h"
#include "Buffer.h"
#include "Image.h"
#include "DescriptorSets.h"
