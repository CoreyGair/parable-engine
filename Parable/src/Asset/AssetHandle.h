#pragma once

namespace Parable
{

    
class AssetStateBlock;

class AssetHandle 
{
private:
    AssetStateBlock* m_state_block;

public:
    AssetHandle() = default;
    AssetHandle(AssetStateBlock* state_block);

    AssetHandle(AssetHandle& other) : AssetHandle(other.m_state_block) {}
    AssetHandle(AssetHandle&& other) : AssetHandle(other.m_state_block) 
    {
        other.m_state_block = nullptr;
    }

    ~AssetHandle();

    AssetHandle& operator=(const AssetHandle& other)
    {
        m_state_block = other.m_state_block;
        return *this;
    }
    AssetHandle& operator=(AssetHandle&& other)
    {
        m_state_block = other.m_state_block;
        other.m_state_block = nullptr;
        return *this;
    }
    
    operator bool() const
    {
        return m_state_block;
    }

    AssetStateBlock* get_state_block() const { return m_state_block; }
};


}
