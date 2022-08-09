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
    RenderLayer(std::unique_ptr<Renderer> renderer) : Layer("RenderLayer"), m_renderer(std::move(renderer)) {}

    void on_update() override;
    void on_event(Event* e) override;

private:
    bool on_window_resize(WindowResizeEvent& e);
    bool on_window_minimise(WindowMinimiseEvent& e);

    std::unique_ptr<Renderer> m_renderer;
};


}