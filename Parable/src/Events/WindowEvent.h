#pragma once

#include "Core/Base.h"
#include "Events/Event.h"

#include <sstream>

namespace Parable
{


class WindowMinimiseEvent : public Event
{
public:
    EVENT_CLASS_CATEGORY(EventCategoryWindow)
    EVENT_CLASS_TYPE(WindowMinimised) 

    WindowMinimiseEvent(bool is_minimised) : minimised(is_minimised) {}
    const bool minimised;

    std::string to_string() const override { std::stringstream out_stream; out_stream << get_name() << ": Minimised=" << minimised; return out_stream.str();}
};

class WindowFocusEvent : public Event
{
public:
    EVENT_CLASS_CATEGORY(EventCategoryWindow)
    EVENT_CLASS_TYPE(WindowFocused) 

    WindowFocusEvent(bool is_focused) : focused(is_focused) {}
    const bool focused;

    std::string to_string() const override { std::stringstream out_stream; out_stream << get_name() << ": Focused=" << focused; return out_stream.str();}
};

class WindowCloseEvent : public Event
{
public:
    EVENT_CLASS_CATEGORY(EventCategoryWindow)
    EVENT_CLASS_TYPE(WindowClosed) 
};

class WindowResizeEvent : public Event
{
public:
    int get_width() { return m_width; }
    int get_height() { return m_height; }

    EVENT_CLASS_CATEGORY(EventCategoryWindow)
    EVENT_CLASS_TYPE(WindowResized) 

    WindowResizeEvent(int width, int height) : m_width(width), m_height(height) {}

    std::string to_string() const override { std::stringstream out_stream; out_stream << get_name() << ": Width=" << m_width << ", Height=" << m_height; return out_stream.str();}

private:
    unsigned int m_width;
    unsigned int m_height;
};


}