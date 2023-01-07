#include "RenderLayer.h"

#include "Events/WindowEvent.h"

namespace Parable
{


void RenderLayer::on_update()
{
    m_renderer->on_update();
}

void RenderLayer::on_event(Event* e)
{
    EventDispatcher dispatcher (e);
    dispatcher.dispatch<WindowResizeEvent>(PBL_BIND_MEMBER_EVENT_HANDLER(RenderLayer::on_window_resize));
}

bool RenderLayer::on_window_resize(WindowResizeEvent& e)
{
    m_renderer->on_resize(e.get_width(), e.get_height());
    
    return true;
}

bool RenderLayer::on_window_minimise(WindowMinimiseEvent& e)
{
    m_renderer->on_minimise(e.minimised);

    return true;
}


}