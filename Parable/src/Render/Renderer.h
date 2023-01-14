#pragma once

#include "Core/Base.h"

#include "Handles.h"
#include <glm/fwd.hpp>

class GLFWwindow;


namespace Parable
{


class RenderLayer;

class MeshVk;

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

    virtual MeshHandle load_mesh(std::string path) = 0;
    virtual void set_mesh_transform(MeshHandle handle, glm::mat4& transform) = 0;

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