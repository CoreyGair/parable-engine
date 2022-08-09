#pragma once

#include <vulkan/vulkan.h>
#include "Util/ReadFile.h"

namespace Parable::Vulkan
{


VkShaderModule load_shader(VkDevice device, const std::string& filename);


} 
