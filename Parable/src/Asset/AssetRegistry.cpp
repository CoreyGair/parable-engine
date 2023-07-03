#include "AssetRegistry.h"


namespace Parable
{


std::map<AssetDescriptor, std::unique_ptr<AssetLoadInfo>> AssetRegistry::descriptor_to_load_info;

void AssetRegistry::init()
{
    // for now, just statically populating test stuff
    descriptor_to_load_info.emplace(0, std::make_unique<MeshLoadInfo>("D:\\parable-engine\\Parable\\src\\Renderer\\Models\\unit_cube.obj"));
    
    descriptor_to_load_info.emplace(1, std::make_unique<TextureLoadInfo>("D:\\parable-engine\\Parable\\src\\Renderer\\Textures\\test_grid.png"));
    descriptor_to_load_info.emplace(2, std::make_unique<TextureLoadInfo>("D:\\parable-engine\\Parable\\src\\Renderer\\Textures\\viking_room.png"));
}


}
