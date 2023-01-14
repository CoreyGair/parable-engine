#pragma once

#include <pblpch.h>


namespace glm
{
    class mat4;
}

namespace Parable
{


struct MeshHandle
{
    const uint64_t chunk;
    const uint64_t mesh;
};


}