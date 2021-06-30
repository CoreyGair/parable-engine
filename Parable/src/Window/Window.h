#pragma once

#include "Core/Base.h"
#include "Events/Event.h"

class GLFWwindow;

namespace Parable
{

// may be more efficient as pure lambda, but this is easy
using EventCallbackFn = std::function<void(Event::EventUPtr)>;

struct WindowData
{
    int width;
    int height;
    std::string name;
    bool fullscreen;
    EventCallbackFn event_callback;
};

class Window
{
private:
    WindowData m_window_data;

    GLFWwindow* m_glfw_window;

    EventCallbackFn m_event_callback;

public:
    Window(int width, int height, std::string name, bool fullscreen);
    ~Window();

    void on_update();

    void set_app_event_callback(EventCallbackFn callback) { m_window_data.event_callback = std::move(callback); }

    static void glfw_error_callback(int error, const char* description);
};


}