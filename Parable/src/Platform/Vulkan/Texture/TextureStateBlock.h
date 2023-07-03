#pragma once

#include "Texture.h"

#include "Asset/AssetState.h"

namespace Parable::Vulkan
{


class TextureStateBlock : public Parable::AssetStateBlock
{
private:
    Texture m_texture;

public:
    TextureStateBlock(Texture texture) : m_texture(texture) {}

    Texture& get_texture() { return m_texture; }
    void set_texture(Texture&& texture) { m_texture = std::move(texture); }
};


}
