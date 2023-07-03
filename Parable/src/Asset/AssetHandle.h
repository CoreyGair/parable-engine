#pragma once

namespace Parable
{

    
class AssetStateBlock;

class AssetHandle 
{
private:
    AssetStateBlock* m_state_block = nullptr;

public:
    AssetHandle() = default;
    AssetHandle(AssetStateBlock* state_block);

    AssetHandle(const AssetHandle& other) : AssetHandle(other.m_state_block) {}
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

    bool operator==(const AssetHandle& rhs)
    {
        return equals(rhs);
    }

    bool equals(const AssetHandle& other) { return m_state_block == other.m_state_block; }
    
    operator bool() const
    {
        return m_state_block;
    }

    AssetStateBlock* get_state_block() const { return m_state_block; }

    bool is_loaded() const;
};


}
