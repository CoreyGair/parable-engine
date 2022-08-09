#include "Window/Window.h"

#include "Events/WindowEvent.h"
#include "Events/InputEvent.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace Parable
{


/**
 * Construct a new Window.
 * 
 * Essentially just initialises a bunch of glwf stuff.
 * 
 * @param width window widith in pixels
 * @param height window height in pixels
 * @param name window name
 * @param fullscreen should the window start in fullscreen?
 */
Window::Window(int width, int height, std::string name, bool fullscreen)
{
    // init glfw
    int init_result = glfwInit();
    PBL_CORE_ASSERT_MSG(init_result, "Could not init GLFW in Window constructor.");

    // tell we are using vulkan, not opengl
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    glfwSetErrorCallback(glfw_error_callback);

    m_glfw_window = glfwCreateWindow(width, height, name.c_str(), fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
    PBL_CORE_ASSERT_MSG(m_glfw_window, "Window creation failed!");

    // at first these were member vars, but this would require capturing &this in the callback lambdas below
    // instead, we attach the data object to glfw window user data pointer so we can retrieve it in callback
    m_window_data.width = width;
    m_window_data.height = height;
    m_window_data.name = name;  
    m_window_data.fullscreen = fullscreen;
    m_window_data.focused = true; // by default, new glfw windows are focused
    m_window_data.minimised = false;


    glfwSetWindowUserPointer(m_glfw_window, &m_window_data);

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
            // add one to button as our input implementation uses the button codes offset by +1
            event = std::make_unique<MouseBtnPressedEvent>(button+1);
        } 
        else 
        {
            // add one to button as our input implementation uses the button codes offset by +1
            event = std::make_unique<MouseBtnReleasedEvent>(button+1);
        }

        window_data.event_callback(std::move(event));
    });

    glfwSetScrollCallback(m_glfw_window, [](GLFWwindow* window, double xoffset, double yoffset)
    {
        WindowData& window_data = *(WindowData*)glfwGetWindowUserPointer(window);

        Event::EventUPtr event = std::make_unique<MouseScrolledEvent>(yoffset);
        window_data.event_callback(std::move(event));
    });

    glfwSetWindowFocusCallback(m_glfw_window, [](GLFWwindow* window, int focused)
    {
        bool f = focused == GL_TRUE;

        WindowData& window_data = *(WindowData*)glfwGetWindowUserPointer(window);
        window_data.focused = f;

        Event::EventUPtr event = std::make_unique<WindowFocusEvent>(f);
        window_data.event_callback(std::move(event));
    });

    glfwSetWindowIconifyCallback(m_glfw_window, [](GLFWwindow* window, int iconified)
    {
        bool m = iconified == GL_TRUE;

        WindowData& window_data = *(WindowData*)glfwGetWindowUserPointer(window);
        window_data.minimised = m;
        
        Event::EventUPtr event = std::make_unique<WindowMinimiseEvent>(m);
        window_data.event_callback(std::move(event));
    });

}

Window::~Window()
{
    glfwDestroyWindow(m_glfw_window);
    
    // must terminate glfw
    glfwTerminate();
}

/**
 * Tells the GLFW window to process all events, thereby adding them to the event queue in Application.
 */
void Window::on_update()
{
    glfwPollEvents();
}

void Window::glfw_error_callback(int error, const char* description)
{
    PBL_CORE_ERROR("GLFW encountered an error. Error ({}): {}", error, description);
}


}