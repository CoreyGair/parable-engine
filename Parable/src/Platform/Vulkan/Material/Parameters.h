#pragma once

#include "pblpch.h"

#include <vulkan/vulkan.hpp>

namespace Parable::Vulkan
{


class MaterialParameters
{
private:
    // vector<SampledTexture> textures
    // vector<float> floats - things like offsets, smoothness, etc
    // vector<vec4> colors - emissive, 

    // TODO: maybe want to figure out a better way to do the bindings here
    //      atm is just assuming index-based: textures[0] binds to the first available tex slot in the template etc.
}


}