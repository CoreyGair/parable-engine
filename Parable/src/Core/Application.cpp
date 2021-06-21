#include "pblpch.h"

#include "Application.h"

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

void Application::on_event(Event::EventUPtr e)
{
    m_event_buffer.push(std::move(e));
}

}