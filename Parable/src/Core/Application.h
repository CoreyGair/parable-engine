#pragma once

#include "pblpch.h"

#include "Core/Base.h"
#include "Core/LayerStack.h"

#include "Events/EventBuffer.h"

#include "ECS/ECS.h"

#include "Window/Window.h"


int main(int argc, char** argv);

namespace Parable
{


/**
 * Manages the overall state for the application.
 * 
 * Recieves events from the window and passes them to application layers.
 * Handles update cycles.
 * 
 * 
 */
class Application
{

    public:
        static Application* s_instance; /// Singleton application pointer 
        static Application& get_instance() { return *s_instance; }

        Application();
        virtual ~Application() {}

        // called each frame, update each layer
        void on_update();
    
    protected:
        void push_layer(Layer* layer);

        /**
         * Buffer of events to be processed.
         */
        EventBuffer m_event_buffer;
        /**
         * Engine layers, which consume events.
         */
        LayerStack m_layer_stack;

        /**
         * The engine ecs.
         */
        //ECS::ECS m_ecs;

    private:
        void run();

        void on_event(Event::EventUPtr e);
        void process_events();

        /**
         * The main application window.
         */
        std::unique_ptr<Window> m_window;

        bool m_running = true;
        bool m_minimised = false;
        friend int ::main(int argc, char** argv);

};

///
/// Create the singleton application object
///
/// To be defined in the CLIENT
///
Application* create_application();

}