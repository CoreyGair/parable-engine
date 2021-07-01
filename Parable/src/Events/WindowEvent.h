#pragma once

#include "Core/Base.h"
#include "Events/Event.h"

#include <sstream>

namespace Parable
{


class WindowCloseEvent : public Event
{
public:
    EVENT_CLASS_CATEGORY(EventCategoryWindow)
    EVENT_CLASS_TYPE(WindowClose) 
};

class WindowResizeEvent : public Event
{
public:
    int get_width() { return m_width; }
    int get_height() { return m_height; }

    EVENT_CLASS_CATEGORY(EventCategoryWindow)
    EVENT_CLASS_TYPE(WindowResize) 

    WindowResizeEvent(int width, int height) : m_width(width), m_height(height) {}

    std::string to_string() const override { std::stringstream out_stream; out_stream << get_name() << ": Width=" << m_width << ", Height=" << m_height; return out_stream.str();}

private:
    unsigned int m_width;
    unsigned int m_height;
};


}