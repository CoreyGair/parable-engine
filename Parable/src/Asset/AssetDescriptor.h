#pragma once

#include "pblpch.h"


namespace Parable
{


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
    AssetDescriptor(std::string&& path) : m_path(std::move(path)) {}

    operator const std::string&() const { return m_path; }
    const std::string& get_path() const { return m_path; }
};

}
