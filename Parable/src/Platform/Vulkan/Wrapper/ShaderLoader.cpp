#include "ShaderLoader.h"

#include "VulkanExceptions.h"

#include "Util/ReadFile.h"

namespace Parable::Vulkan
{


vk::ShaderModule load_shader(vk::Device& device, const std::string& filename)
{
    std::vector<char> code = Util::read_file(filename);

    vk::ShaderModuleCreateInfo createInfo(
        {},
        code.size(),
        reinterpret_cast<uint32_t*>(code.data())
    );

    return device.createShaderModule(createInfo);
}


}