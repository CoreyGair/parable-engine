#pragma once

#include "pblpch.h"

#include "Core/Base.h"

namespace Parable
{


enum class AssetState
{
    Unloaded,
    Loading,
    Loaded
};

class Asset;

template<class T>
concept IsAsset = std::derived_from<T, Asset>;

template<IsAsset T>
class Handle;

/**
 * AssetDescriptor describes an asset in the game files.
 * 
 * Essentially a handle to a not yet loaded Asset of some kind.
 */
class AssetDescriptor
{
private:
    std::string m_path;

public:
    template<IsAsset T>
    Handle<T> load()
    {
        return AssetStore<T>::retrieve_asset_handle(this);
    }
};

/**
 * Interface defining functionality required of an Asset.
 * 
 * An "Asset" is some data outside the game code which is used by the game/engine.
 * Examples are meshes, textures, audio files, materials, etc.
 * 
 * The designed usage for Assets is as follows:
 * - Start with an AssetDescriptor, which describes some asset on disk.
 * - Use the AssetDescriptor to obtain a Handle<T>, where T is a concrete Asset (this possibly loads the Asset to memory if it is not already).
 * - The Asset/Handle classes take care of reference tracking and unloading dead objects.
 */
class Asset
{
private:
    /**
     * The number of live Handles pointing to this Asset.
     */
    int m_references = 0;

    // The handle class needs to modify reference counts and load/unload operations.
    template<IsAsset T>
    friend class Handle;

    /**
     * To be called by Handle when the last handle to this Asset is destroyed.
     */
    void on_death()
    {
        PBL_CORE_ASSERT(m_references == 0);

        // for now, unload the asset right away
        // TODO: want to add some zombie behaviour,
        // where assets are not unloaded right away (help with temporal locality of load/unloads)
        // Make Asset-derived types define a "DeathBehaviour" (probs with an enum of death strategies) (STRATEGY PATTERN?)
        // which AssetStore reads to determine how to act in this func
        // e.g. DeathBehavior::UnloadInstant unloads right away,
        // but ::ZombieUntilNotify (bad name) keeps Asset loaded until some notifier is called for that asset class
        // (e.g. if need to clear some vram, can notify to delete zombie mesh + texture assets)

        unload();
    }

protected:
    /**
     * @brief Load the asset from disk.
     * 
     * May modify the state of this Asset asynchronously.
     * 
     * TODO: have this return bool, and implementations check the structure of descriptor.
     *          (for use later when AssetDescriptor is not just a string)
     *          return false if descriptor is invalid,
     *          true indicates the asset will load
     * 
     * @param descriptor Contains information on the Asset on disk.
     */
    virtual void load(AssetDescriptor descriptor) = 0;

    /**
     * @brief Unload this asset, freeing any system resources it occupied (e.g. memory).
     * 
     * The state of this Asset will synchronously change to Unloaded, but actual freeing of resources may occur asynchronously. (TODO: is this needed? e.g. free resources at frame end or smth?)
     */
    virtual void unload() = 0;
    
    AssetState m_state = AssetState::Unloaded;

public:
    Asset() = default;
    ~Asset()
    {
        PBL_CORE_ASSERT(m_references == 0);
        PBL_CORE_ASSERT(m_state == AssetState::Unloaded);
    }
    
    AssetState get_state() { return m_state; }
};


/**
 * AssetStore provides static storage for concrete Asset objects.
 * 
 * @tparam T the concrete Asset type to retrieve.
 */
template<IsAsset T>
class AssetStore
{
private:
    /**
     * A static cache of Asset objects, keyed by the AssetDescriptor used to load them.
     */
    static std::map<AssetDescriptor,UPtr<Asset>> loaded_assets;

public:
    /**
     * Retrieves a handle to the Asset described by an AssetDescriptor.
     * 
     * Note this will invoke asset loading if the desired Asset is currently unloaded.
     *
     * Assets pointed to by newly created Handles may be in a "loading" state for a while before being fully loaded.
     * 
     * @param descriptor 
     */
    static void retrieve_asset_handle(AssetDescriptor& descriptor)
    {
        if (auto it = loaded_assets.find(descriptor); it != loaded_assets.end())
        {
            T& asset = static_cast<T*>(it->first.get());
            if (asset.get_state() == AssetState::Unloaded)
            {
                asset.load(descriptor);
            }
            return Handle<T>(asset);
        }

        // the asset is not cached (has never been loaded before)
        // need to create it
        UPtr<T> new_asset = std::make_unique<T>();
        new_asset->load(descriptor);
        loaded_assets.insert(descriptor, std::move(new_asset));

        return Handle<T>(*new_asset);
    }

    /**
     * Destroys all Assets.
     */
    static void clear()
    {
        loaded_assets.clear();
    }
};

/***
 * A handle to a loaded (in-memory) Asset.
*/
template<IsAsset T>
class Handle
{
private:
    Asset& m_asset;

public:
    Handle<T>(T& asset) : m_asset(asset) { m_asset->m_references++; }
    Handle<T>(const Handle<T>& other) : Handle<T>(other.m_asset) {}
    Handle<T>(Handle<T>&& other) : m_asset(other.m_asset) {}

    ~Handle<T>()
    {
        PBL_CORE_ASSERT(m_asset->m_references > 0)

        m_asset.m_references--;

        if (m_asset.m_references == 0)
        {
            m_asset.on_death();
        }
    }

    // TODO: conversions to T&
};


} 
