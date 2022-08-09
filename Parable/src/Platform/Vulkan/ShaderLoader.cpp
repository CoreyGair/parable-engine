#include "ShaderLoader.h"

#include "VulkanExceptions.h"

#include "Util/ReadFile.h"

namespace Parable::Vulkan
{


VkShaderModule load_shader(VkDevice device, const std::string& filename)
{
    std::vector<char> code = Util::read_file(filename);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    VkResult res = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
    if (res != VK_SUCCESS)
    {
        throw VulkanFailedCreateException("shader module", res);
    }

    return shaderModule;
}


}