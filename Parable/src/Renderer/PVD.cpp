/**
 * @file PVD.cpp Defines several pure virtual destructors for Renderer resource interfaces.
 */

#include "Mesh.h"
#include "Texture.h"
#include "Effect.h"
#include "Material.h"

namespace Parable
{
Mesh::~Mesh() {}
Texture::~Texture() {}
Effect::~Effect() {}
Material::~Material() {}
}