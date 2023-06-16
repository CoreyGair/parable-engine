#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include "Asset.h"

namespace Parable
{


/***
 * A handle to a loaded (in-memory) Asset.
*/
template<IsAsset T>
class Handle
{
private:
    T& m_asset;

public:
    Handle<T>(T& asset) : m_asset(asset) { m_asset.on_handle_create(); }
    Handle<T>(const Handle<T>& other) : Handle<T>(other.m_asset) {}
    Handle<T>(Handle<T>&& other) : m_asset(other.m_asset) {}

    ~Handle<T>()
    {
        m_asset.on_handle_destroy();
    }

    // TODO: any other worthwhile converters?
    explicit operator T&() { return m_asset; }
    T& get() { return m_asset; }
};


} 
