#pragma once

#include "Core/Base.h"
#include "Core/Layer.h"

#include "Renderer.h"


namespace Parable
{


class WindowResizeEvent;
class WindowMinimiseEvent;


/**
 * Engine layer responsible for the renderer.
 */
class RenderLayer : public Layer
{
public:
    RenderLayer() : Layer("RenderLayer"), m_renderer(Renderer::get_instance()) {}

    void on_update() override;
    void on_event(Event* e) override;

private:
    bool on_window_resize(WindowResizeEvent& e);
    bool on_window_minimise(WindowMinimiseEvent& e);

    Renderer* m_renderer;
};


}