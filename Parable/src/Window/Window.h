#pragma once

#include "Core/Base.h"
#include "Events/Event.h"

#include <vulkan/vulkan.h>

class GLFWwindow;

namespace Parable
{

// may be more efficient as pure lambda, but this is easy
using EventCallbackFn = std::function<void(Event::EventUPtr)>;

/**
 * Holds data about a glfw window.
 * 
 */
struct WindowData
{
    int width;
    int height;
    std::string name;
    bool fullscreen;
    bool focused;
    bool minimised;
    EventCallbackFn event_callback;
};

/**
 * Manages the glfw window.
 */
class Window
{
private:
    WindowData m_window_data;

    GLFWwindow* m_glfw_window;

public:
    Window(int width, int height, std::string name, bool fullscreen);
    ~Window();

    GLFWwindow* get_glfw_window() const { return m_glfw_window; }

    void on_update();

    void set_app_event_callback(EventCallbackFn callback) { m_window_data.event_callback = std::move(callback); }

    static void glfw_error_callback(int error, const char* description);
};


}