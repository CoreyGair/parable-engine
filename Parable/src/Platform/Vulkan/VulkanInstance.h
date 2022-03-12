#pragma once

#include <vulkan/vulkan.h>

namespace Parable
{


void create_vulkan_instance(VkInstance* instance);

void destroy_vulkan_instance(VkInstance instance);


}