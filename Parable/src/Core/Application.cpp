#include "pblpch.h"

#include "Core/Application.h"
#include "Core/Layer.h"

namespace Parable
{

Application* Application::s_instance = nullptr; 

Application::Application()
{
    s_instance = this;
}

void Application::run()
{

    while(true);

}

// pushes event to event queue
void Application::on_event(Event::EventUPtr e)
{
    m_event_buffer.push(std::move(e));
}

// dispatches all events in queue to layers
void Application::process_events()
{
    while(!m_event_buffer.is_empty())
    {
        Event::EventUPtr e = m_event_buffer.next();
        EventDispatcher dispatcher (std::move(e));
        for(auto it = m_layer_stack.cbegin(); it != m_layer_stack.cend(); ++it)
        {
            if (dispatcher.handled()) return;

            (*it)->on_event(dispatcher);
        }
    }
}

void Application::push_layer(Layer* layer)
{
    m_layer_stack.push(layer);
}

}