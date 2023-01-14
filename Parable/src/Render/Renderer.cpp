#include "Renderer.h"

#include "Platform/Vulkan/Renderer.h"


namespace Parable
{


Renderer* Renderer::instance = nullptr;

void Renderer::Init(GLFWwindow* window)
{
    PBL_CORE_ASSERT(instance == nullptr);
    
    instance = new Vulkan::Renderer(window);
}


}
