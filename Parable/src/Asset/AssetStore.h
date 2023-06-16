#pragma once

#include "pblpch.h"


namespace Parable
{


/**
 * Defines an interface for AssetStore objects (which manage the storage/lookup of Asset implementations), and some helper functionality.
 * 
 * Derived classes should implement the public interface and make use of the protected functions to store and look up new Asset objects.
 * 
 * @tparam T The concrete Asset type to be stored.
 */
template<IsAsset T>
class AssetStore 
{
private:
    std::map<AssetDescriptor, T> m_stored_assets;

protected:
    /**
     * Retrieve a reference to a stored Asset.
     * 
     * @param descriptor The descriptor to look up the Asset for.
     * @return std::optional<T&> contains a reference to the Asset object if it is stored, else empty.
     */
    std::optional<T&> retrieve_stored_asset(AssetDescriptor& descriptor)
    {
        auto it = m_stored_assets.find(descriptor);
        return it == m_stored_assets.end() ? {} : {it->second};
    }

    void clear_stored_assets()
    {
        m_stored_assets.clear();
    }

    /**
     * Create a new Asset object.
     * 
     * To be implemented by concrete AssetStores to properly call the constructor of the concrete Asset type.
     * 
     * @return T The new Asset.
     */
    virtual T create_asset() = 0;

public:
    /**
     * Retrieve a handle to the Asset described in an AssetDescriptor.
     * 
     * Invokes loading on any 
     * 
     * @param desc 
     * @return Handle<T> 
     */
    Handle<T> get_handle(AssetDescriptor descriptor)
    {
        T& asset;

        // first check if we already created this Asset
        auto it = m_stored_assets.find(descriptor);
        if (it != m_stored_assets.end())
        {
            asset = it->second;
        } else {
            // else we need to create it
            auto [it,_] = m_stored_assets.emplace(std::move(create_asset()));
            asset = it->second;
        }

        if (asset.get_state() == AssetState::Unloaded)
        {
            asset.load(descriptor);
        }

        return Handle<T>(asset);
    }
};


}
