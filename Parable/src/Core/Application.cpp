#include "pblpch.h"

#include "Core/Application.h"
#include "Core/Layer.h"

//TEMP FOR TEST
#include "Debug/EventLogLayer.h"
#include "Input/InputLayer.h"

namespace Parable
{

Application* Application::s_instance = nullptr; 

Application::Application()
{
    PBL_ASSERT_MSG(!s_instance, "Application already exists!");
    s_instance = this;

    m_window = std::make_unique<Window>(1600,900,std::string("Parable Engine"), false);
    m_window->set_app_event_callback(PBL_BIND_MEMBER_EVENT_HANDLER(Application::on_event));

    // TEMP TEST LAYERS
    m_layer_stack.push(new Input::InputLayer());
    m_layer_stack.push(new EventLogLayer(0));
    
}

void Application::run()
{

    while(true)
    {
        if (!m_event_buffer.is_empty())
        {
            process_events();
        }

        m_window->on_update();
    }

}

// call update on each layer
void Application::on_update()
{
    for(auto it = m_layer_stack.cbegin(); it != m_layer_stack.cend(); ++it)
    {
        (*it)->on_update();
    }
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
        for(auto it = m_layer_stack.crbegin(); it != m_layer_stack.crend(); ++it)
        {
            if (e->handled) return;

            (*it)->on_event(e.get());
        }
    }
}

void Application::push_layer(Layer* layer)
{
    m_layer_stack.push(layer);
}


}