#pragma once

namespace Parable
{


class ResourceState;
template<class T>
class ResourceStorageBlock;

/**
 * An opaque handle to a resources state.
 */
class StateHandle
{
private:
    ResourceState* m_state_block = nullptr;

protected:
    ResourceState* get_state_block() { return m_state_block; }
    const ResourceState* get_state_block() const { return m_state_block; }

public:
    StateHandle() = default;
    StateHandle(ResourceState& state_block);

    ~StateHandle();

    StateHandle(const StateHandle& other);
    StateHandle& operator=(const StateHandle& other);

    StateHandle(StateHandle&& other);
    StateHandle& operator=(StateHandle&& other);

    bool operator==(const StateHandle& rhs) const
    {
        return m_state_block == rhs.m_state_block;
    }

    operator bool() const { return m_state_block; }

    bool is_loaded() const;
};

/**
 * An opaque handle to a Resource and its state.
 * 
 * @tparam ResourceType The type of resource pointed to.
 */
template<class ResourceType>
class Handle : public StateHandle
{
private:
    /**
     * Checks if this handle is null for a dereference, throwing the corresponding exception if null.
     */
    void dereference_null_check() const
    {
        if (!get_state_block())
        {
            throw std::exception("NullHandleDereferenceException");
        }
    }

    ResourceStorageBlock<ResourceType>* get_storage_block()
    {
        return static_cast<ResourceStorageBlock<ResourceType>*>(get_state_block());
    }
    const ResourceStorageBlock<ResourceType>* get_storage_block() const
    {
        return static_cast<const ResourceStorageBlock<ResourceType>*>(get_state_block());
    }

public:
    Handle() = default;
    Handle(ResourceStorageBlock<ResourceType>& resource_block) 
        : StateHandle(static_cast<ResourceState&>(resource_block))
    {}
    ~Handle() = default;

    /**
     * Dereferences the handle to use the resource.
     */
    ResourceType& operator*()
    {
        dereference_null_check();
        return get_storage_block()->get_resource();
    }
    const ResourceType& operator*() const
    {
        dereference_null_check();
        return get_storage_block()->get_resource();
    }
    ResourceType* operator->()
    {
        dereference_null_check();
        return &(get_storage_block().get_resource());
    }
    ResourceType* operator->() const
    {
        dereference_null_check();
        return &(get_storage_block().get_resource());
    }
    
    bool operator==(const Handle<ResourceType>& rhs)
    {
        return static_cast<const StateHandle&>(*this) == static_cast<const StateHandle&>(rhs);
    }
    bool operator!=(const Handle<ResourceType>& rhs)
    {
        return static_cast<const StateHandle&>(*this) != static_cast<const StateHandle&>(rhs);
    }
};


}
