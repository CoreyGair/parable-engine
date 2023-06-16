#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include "AssetDescriptor.h"

namespace Parable
{


// TODO: in future allow "preloading"
// will need some specific function in AssetStore to preload
// - creates the Asset as normal
// - sets state to a special ::Preloaded state
// - this prevents it from being unloaded (except by complete clear of AssetStore.clear_stored_assets())
// - until first handle retrieved, when state is set to normal ::Loaded
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
 * Interface defining functionality required of an Asset.
 * 
 * An "Asset" is some data outside the game code which is used by the game/engine.
 * Examples are meshes, textures, audio files, materials, etc.
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

    void on_handle_create() { ++m_references; }
    void on_handle_destroy()
    {
        PBL_CORE_ASSERT(m_references > 0);
        if (--m_references == 0)
        {
            // TODO: zombie behaviour here?
            // set state to zombie
            // then AssetStore can scan for state == ::Zombie to find ones to actually unload
            unload();
        }
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
     * TODO: will this be async with regards to state?
     * 
     * @param descriptor Contains information about the Asset on disk.
     */
    virtual void load(AssetDescriptor descriptor) = 0;

    /**
     * @brief Unload this asset, freeing any system resources it occupied (e.g. memory).
     * 
     * The state of this Asset will synchronously change to Unloaded, but actual freeing of resources may occur asynchronously. (TODO: is this needed? e.g. free resources at frame end or smth?)
     */
    virtual void unload() = 0;
    
    AssetState m_state;

public:
    // Not an RAII type due to the ::Unloaded state
    Asset(AssetDescriptor descriptor) : m_state(AssetState::Unloaded) {}
    ~Asset()
    {
        PBL_CORE_ASSERT(m_references == 0);
        // TODO: may want to just silently unload asset instead?
        // unless loading state, then do what
        PBL_CORE_ASSERT(m_state == AssetState::Unloaded);
    }
    
    AssetState get_state() { return m_state; }
};


} 
