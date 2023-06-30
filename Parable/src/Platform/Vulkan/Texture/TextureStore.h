#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include "Texture.h"

#include "Renderer/Handles.h"

#include "Asset/AssetState.h"
#include "Asset/AssetDescriptor.h"

namespace Parable::Vulkan
{


class TextureStateBlock : public Parable::AssetStateBlock
{
private:
    Texture m_texture;

public:
    Texture& get_texture() { return m_texture; }
};

/**
 * Handles the storage and lookup of Vulkan Texture resources.
 */
class TextureStore
{
private:
    std::map<AssetDescriptor,TextureStateBlock> m_state_blocks;

    

public:

    Parable::TextureHandle load(AssetDescriptor descriptor);

    Texture& get_texture(Parable::TextureHandle& handle);
};


}
