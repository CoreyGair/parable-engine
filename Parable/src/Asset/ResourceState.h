#pragma once

#include "Core/Base.h"

namespace Parable
{


enum class ResourceLoadState
{
    Unloaded,
    Loading,
    Loaded
};

/**
 * Represents the state of a single Resource.
 */
class ResourceState
{
private:
    /**
     * The count of live Handle's pointing to this.
     */
    int m_reference_count = 0;

    ResourceLoadState m_load_state = ResourceLoadState::Unloaded;

public:
    ~ResourceState()
    {
        // TODO: is this needed or desirable? also should there be an assertion on load_state??
        PBL_CORE_ASSERT(m_reference_count == 0);
    }

    void increment_ref_count()
    {
        m_reference_count++;
    }
    void decrement_ref_count()
    {
        PBL_CORE_ASSERT(m_reference_count > 0);
        m_reference_count--;
    }
    int get_ref_count() const { return m_reference_count; }

    ResourceLoadState get_load_state() const { return m_load_state; }
    void set_load_state(ResourceLoadState state) { m_load_state = state; }
};

/**
 * A block of data storing a Resource with its ResourceState.
 * 
 * @tparam ResourceType The concrete type to store in the block.
 */
template<class ResourceType>
class ResourceStorageBlock : public ResourceState
{
private:
    std::unique_ptr<ResourceType> m_resource;

public:
    ResourceStorageBlock() = default;
    ResourceStorageBlock(std::unique_ptr<ResourceType> resource) : m_resource(std::move(resource)) {}

    ResourceType& get_resource() { PBL_CORE_ASSERT(m_resource); return *m_resource; }
    void set_resource(std::unique_ptr<ResourceType> resource) { m_resource = std::move(resource); }
};


}
