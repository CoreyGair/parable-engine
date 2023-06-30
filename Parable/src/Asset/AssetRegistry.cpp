#include "AssetRegistry.h"


namespace Parable
{


void AssetRegistry::init()
{
    // for now, just statically populating test stuff
    descriptor_to_load_info.emplace(0, std::make_unique<MeshLoadInfo>("D:\\parable-engine\\Parable\\src\\Render\\Models\\unit_cube.obj"));
    
    descriptor_to_load_info.emplace(1, std::make_unique<TextureLoadInfo>("D:\\parable-engine\\Parable\\src\\Render\\Textures\\texture.jpg"));
    descriptor_to_load_info.emplace(2, std::make_unique<TextureLoadInfo>("D:\\parable-engine\\Parable\\src\\Render\\Textures\\viking_room.png"));
}


}
