#pragma once

#include <vulkan/vulkan.hpp>
#include "Util/ReadFile.h"

namespace Parable::Vulkan
{


vk::ShaderModule load_shader(vk::Device& device, const std::string& filename);


} 
