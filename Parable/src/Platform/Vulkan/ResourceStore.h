#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include "Asset/AssetDescriptor.h"
#include "Asset/Handle.h"
#include "Asset/ResourceState.h"


namespace Parable::Vulkan
{


class Loader;
class LoadTask;

/**
 * ResourceLoader passes LoadTasks to a loader for Vulkan resources.
 */
class ResourceLoader
{
private:
    Loader& m_loader;

protected:
    void submit_load_task(std::unique_ptr<LoadTask> task);
    ResourceLoader(Loader& loader) : m_loader(loader) {}
};

/**
 * Handles the storage and lookup of Vulkan resources.
 */
template<class ResourceType>
class ResourceStore : public ResourceLoader
{
private:
    /**
     * Maps previously loaded Resources from their descriptors for lookup.
     */
    std::map<AssetDescriptor,ResourceStorageBlock<ResourceType>> m_descriptor_resource_map;

protected:
    /**
     * @brief Create a new LoadTask object for a given AssetDescriptor.
     * 
     * @param descriptor The AssetDescriptor to create a LoadTask for.
     * @param storage_block The storage block in which to store the new resource.
     * @return std::unique_ptr<LoadTask> 
     */
    virtual std::unique_ptr<LoadTask> create_load_task(AssetDescriptor descriptor, ResourceStorageBlock<ResourceType>& storage_block) = 0;

public:
    ResourceStore(Loader& loader) : ResourceLoader(loader) {}

    /**
     * @brief Get a handle to a resource, loading it if it is not already loaded.
     * 
     * @param descriptor The descriptor to the asset to load.
     * @return Handle<ResourceType> 
     */
    Handle<ResourceType> load(AssetDescriptor descriptor)
    {
        // first see if we already have this resource
        // TODO: what if the state is ::Unloaded? Should initiate loading but reuse the storage block
        auto hint = m_descriptor_resource_map.lower_bound(descriptor);
        if (hint != m_descriptor_resource_map.end() && hint->first == descriptor)
        {
            return Handle<ResourceType>(hint->second);
        }
        
        // its state block goes into the Loading state until the mesh data is uploaded to GPU buffers
        auto it = m_descriptor_resource_map.emplace_hint(hint, descriptor, ResourceStorageBlock<ResourceType>());
        it->second.set_load_state(ResourceLoadState::Loading);

        // now we must submit a load task to copy the mesh data to gpu buffers
        std::unique_ptr<LoadTask> load_task = create_load_task(descriptor, it->second);
        submit_load_task(std::move(load_task));

        return Handle<ResourceType>(it->second);
    }
};


}
