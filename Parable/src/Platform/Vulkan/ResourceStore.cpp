#include "ResourceStore.h"

#include "pblpch.h"
#include "Core/Base.h"

#include "Loader/Loader.h"

namespace Parable::Vulkan
{


void ResourceLoader::submit_load_task(std::unique_ptr<LoadTask> task)
{
    m_loader.submit_task(std::move(task));
}


}