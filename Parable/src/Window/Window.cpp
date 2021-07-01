#include "Window/Window.h"

#include "Events/WindowEvent.h"
#include "Events/InputEvent.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Parable
{


Window::Window(int width, int height, std::string name, bool fullscreen)
{
    // init glfw
    PBL_ASSERT_MSG(glfwInit(), "Could not init GLFW in Window constructor.");

    // min opengl version supported
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    glfwSetErrorCallback(glfw_error_callback);

    m_glfw_window = glfwCreateWindow(width, height, name.c_str(), fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
    PBL_ASSERT_MSG(m_glfw_window, "Window creation failed!");

    // at first these were member vars, but this would require capturing &this in the callback lambdas below
    // instead, we attach the data object to glfw window user data pointer so we can retrieve it in callback
    m_window_data.width = width;
    m_window_data.height = height;
    m_window_data.name = name;
    m_window_data.fullscreen = fullscreen;

    glfwSetWindowUserPointer(m_glfw_window, &m_window_data);

    glfwMakeContextCurrent(m_glfw_window);
    int context_success = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    PBL_ASSERT_MSG(context_success, "Failed to init OpenGL context with GLAD!")

    // set event callbacks
    glfwSetWindowCloseCallback(m_glfw_window, [](GLFWwindow* window)
    {
        WindowData& window_data = *(WindowData*)glfwGetWindowUserPointer(window);
        Event::EventUPtr event = std::make_unique<WindowCloseEvent>();
        window_data.event_callback(std::move(event));
    });

    glfwSetWindowSizeCallback(m_glfw_window, [](GLFWwindow* window, int width, int height)
    {
        WindowData& window_data = *(WindowData*)glfwGetWindowUserPointer(window);
        window_data.width = width;
        window_data.height = height;

        Event::EventUPtr event = std::make_unique<WindowResizeEvent>(width, height);
        window_data.event_callback(std::move(event));
    });

    glfwSetKeyCallback(m_glfw_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        WindowData& window_data = *(WindowData*)glfwGetWindowUserPointer(window);

        Event::EventUPtr event;
        switch(action)
        {
            case GLFW_PRESS:
                event = std::make_unique<KeyPressedEvent>(key);
                break;
            case GLFW_REPEAT:
                event = std::make_unique<KeyRepeatedEvent>(key);
                break;
            case GLFW_RELEASE:
                event = std::make_unique<KeyReleasedEvent>(key);
                break;
        }
        window_data.event_callback(std::move(event));
    });

    glfwSetCursorPosCallback(m_glfw_window, [](GLFWwindow* window, double xpos, double ypos)
    {
        WindowData& window_data = *(WindowData*)glfwGetWindowUserPointer(window);

        Event::EventUPtr event = std::make_unique<MouseMovedEvent>(xpos, ypos);
        window_data.event_callback(std::move(event));
    });

    glfwSetCursorEnterCallback(m_glfw_window, [](GLFWwindow* window, int entered)
    {
        WindowData& window_data = *(WindowData*)glfwGetWindowUserPointer(window);

        Event::EventUPtr event;
        if (entered)
        {
            event = std::make_unique<MouseEnterEvent>();
        } 
        else 
        {
            event = std::make_unique<MouseExitEvent>();
        }

        window_data.event_callback(std::move(event));
    });

    glfwSetMouseButtonCallback(m_glfw_window, [](GLFWwindow* window, int button, int action, int mods)
    {
        WindowData& window_data = *(WindowData*)glfwGetWindowUserPointer(window);

        Event::EventUPtr event;
        if (action == GLFW_PRESS)
        {
            event = std::make_unique<MouseBtnPressedEvent>(button);
        } 
        else 
        {
            event = std::make_unique<MouseBtnReleasedEvent>(button);
        }

        window_data.event_callback(std::move(event));
    });

    glfwSetScrollCallback(m_glfw_window, [](GLFWwindow* window, double xoffset, double yoffset)
    {
        WindowData& window_data = *(WindowData*)glfwGetWindowUserPointer(window);

        Event::EventUPtr event = std::make_unique<MouseScrolledEvent>(yoffset);
        window_data.event_callback(std::move(event));
    });

}

Window::~Window()
{
    glfwDestroyWindow(m_glfw_window);
    // must terminate glfw
    glfwTerminate();
}

void Window::on_update()
{
    glfwPollEvents();
}

void Window::glfw_error_callback(int error, const char* description)
{
    PBL_CORE_ERROR("GLFW encountered an error. Error ({}): {}", error, description);
}


}