#pragma once

#include "Core/Base.h"

#include <glm/fwd.hpp>

#include "Asset/Handle.h"
#include "Asset/AssetDescriptor.h"

class GLFWwindow;

namespace Parable
{


class RenderLayer;

class Mesh;
class Texture;

class Renderer
{
public:
    static void Init(GLFWwindow* window);

    static void Destroy() 
    {
        PBL_CORE_ASSERT(instance != nullptr);

        delete instance;
    }

    static Renderer* get_instance() { PBL_CORE_ASSERT(instance != nullptr); return instance; }

    virtual ~Renderer() {};

    virtual Handle<Mesh> load_mesh(AssetDescriptor descriptor) = 0;

    virtual Handle<Texture> load_texture(AssetDescriptor descriptor) = 0;

    virtual void draw(Handle<Mesh> mesh, Handle<Texture> texture, glm::mat4& transform) = 0;

    virtual void on_update() = 0;

    virtual void on_resize(unsigned int width, unsigned int height) = 0;

    void on_minimise(bool minimised) { m_paused = minimised; }

protected:
    bool m_paused = false;

private:
    static Renderer* instance;

    friend RenderLayer;
};


}