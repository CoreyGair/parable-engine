#include "pblpch.h"

#include "Core/Application.h"
#include "Core/Layer.h"

#include "Window/Window.h"
#include "Events/WindowEvent.h"

#include "ECS/ECS.h"

#include "Renderer/RenderLayer.h"
// could find a way to not have this, let render layer init??
#include "Renderer/Renderer.h"

// TEMP to test renderer
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include "Asset/AssetDescriptor.h"
#include "Asset/Handle.h"
#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"

#include "Asset/AssetRegistry.h"


//TEMP FOR TEST
#include "Debug/EventLogLayer.h"
#include "Input/InputLayer.h"

namespace Parable
{

Application* Application::s_instance = nullptr; 

AssetDescriptor cubeMeshDescriptor = 0;
AssetDescriptor texADescriptor = 1;
AssetDescriptor texBDescriptor = 2;

Handle<Mesh> cubeMesh;
Handle<Mesh> vikingMesh;

Handle<Texture> matA;
Handle<Texture> matB;

glm::mat4 startMatA = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.5f,0.5f,0.5f)), glm::vec3(-1.5f, 0.0f, 0.0f));
glm::mat4 startMatB = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.5f,0.5f,0.5f)), glm::vec3(1.5f, 0.0f, 0.0f));

auto startTime = std::chrono::high_resolution_clock::now();

Application::Application()
{
    AssetRegistry::init();

    PBL_CORE_ASSERT_MSG(!s_instance, "Application already exists!");
    s_instance = this;

    ECS::ECS::ECSBuilder builder;

    builder.set_component_chunk_size(500);
    builder.set_component_chunks_total_size(10000);
    builder.set_entity_component_map_size(2000);

    //m_ecs = builder.create();

    m_window = std::make_unique<Window>(1600,900,std::string("Parable Engine"), false);
    m_window->set_app_event_callback(PBL_BIND_MEMBER_EVENT_HANDLER(Application::on_event));

    Renderer::Init(m_window->get_glfw_window());
    m_layer_stack.push(std::make_unique<RenderLayer>());

    // load test mesh
    cubeMesh = Renderer::get_instance()->load_mesh(4);
    vikingMesh = Renderer::get_instance()->load_mesh(3);

    // load textures to test
    matA = Renderer::get_instance()->load_texture(texADescriptor);
    matB = Renderer::get_instance()->load_texture(texBDescriptor);

    m_layer_stack.push(std::make_unique<Input::InputLayer>());

    m_layer_stack.push(std::make_unique<EventLogLayer>(0));
}

/**
 * Main application loop.
 * 
 * Processes events and updates.
 * 
 */
void Application::run()
{

    // TODO: initialise ecs with component & system types

    while(m_running)
    {
        time.start_frame();

        // TEMP, rotate meshes for test
        auto currentTime = std::chrono::high_resolution_clock::now();
        float elapsedTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        auto currMatA = glm::rotate(startMatA, elapsedTime * glm::radians(90.0f), glm::vec3(0.0f,0.0f,1.0f));
        auto currMatB = glm::rotate(startMatB, -elapsedTime * glm::radians(45.0f), glm::vec3(0.0f,0.0f,1.0f));

        Renderer::get_instance()->draw(cubeMesh, matA, currMatA);
        Renderer::get_instance()->draw(vikingMesh, matB, currMatB);

        // invoke update for the ecs
        //m_ecs.on_update();

        // invoke update across the layer stack
        on_update();

        // get events from glfw window
        m_window->on_update();

        if (!m_event_buffer.is_empty())
        {
            process_events();
        }

        time.end_frame();
    }

}

/**
 * Process a frame update within the engine.
 * 
 * Internally, calls update methods in the ECS and engine layers.
 * 
 * Only override if you know what you're doing, altering this will alter the function of the engine.
 */
void Application::on_update()
{
    for(auto it = m_layer_stack.cbegin(); it != m_layer_stack.cend(); ++it)
    {
        (*it)->on_update();
    }
}

/**
 * Pushes event to event queue
 */
void Application::on_event(Event::EventUPtr e)
{
    m_event_buffer.push(std::move(e));
}

/**
 * Dispatches all events in queue to layers
 */
void Application::process_events()
{
    while(!m_event_buffer.is_empty())
    {
        Event::EventUPtr e = m_event_buffer.next();

        // we handle window closing (and thus application quitting) here
        if (e->get_event_type() == WindowCloseEvent::get_static_type())
        {
            e->handled = true;
            m_running = false;
        }

        for(auto it = m_layer_stack.crbegin(); it != m_layer_stack.crend(); ++it)
        {
            if (e->handled) return;

            (*it)->on_event(e.get());
        }
    }
}

/**
 * Add an engine layer.
 */
void Application::push_layer(UPtr<Layer> layer)
{
    m_layer_stack.push(std::move(layer));
}


}