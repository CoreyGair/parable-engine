#include "pblpch.h"

#include "Application.h"

namespace Parable
{

Application* Application::s_instance = nullptr; 

Application::Application()
{
    s_instance = this;
}

void Application::Run()
{

    while(true);

}

void Application::OnEvent(Event::EventUPtr e)
{
    m_event_buffer.push(std::move(e));
}

}