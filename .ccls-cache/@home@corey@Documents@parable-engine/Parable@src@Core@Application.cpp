#include "pblpch.h"

#include "Application.h"

namespace Parable{

Application* Application::s_instance = nullptr;

Application::Application(){
    s_instance = this;
}

void Application::Run(){
    
    while(true);
    
}

}