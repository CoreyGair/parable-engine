#pragma once

#include "Core/Base.h"
#include "Events/Event.h"

#include "Input/InputCodes.h"

#include <sstream>

namespace Parable
{


//// KEYBOARD EVENTS


/**
 * Abstract class for keyboard input events.
 * 
 */
class KeyEvent : public Event
{
public:
    virtual ~KeyEvent() = 0;

    Input::KeyCode get_key_code() { return m_keycode; }

    KeyEvent(const Input::KeyCode key) : m_keycode(key) {}
    KeyEvent(int key) : m_keycode(static_cast<Input::KeyCode>(key)) {}

protected:
    /**
     * The key which the event is about.
     */
    Input::KeyCode m_keycode;
};


class KeyPressedEvent : public KeyEvent
{
public:
    EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard | EventCategoryButton)
    EVENT_CLASS_TYPE(KeyPressed)
    using KeyEvent::KeyEvent;
    std::string to_string() const override { std::stringstream out_stream; out_stream << get_name() << ": KeyCode=" << (Input::InputCode)m_keycode; return out_stream.str();}
};

class KeyRepeatedEvent : public KeyEvent
{
public:
    EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard)
    EVENT_CLASS_TYPE(KeyRepeated)
    using KeyEvent::KeyEvent;
    std::string to_string() const override { std::stringstream out_stream; out_stream << get_name() << ": KeyCode=" << (Input::InputCode)m_keycode; return out_stream.str();}
};

class KeyReleasedEvent : public KeyEvent
{
public:
    EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard | EventCategoryButton)
    EVENT_CLASS_TYPE(KeyReleased)
    using KeyEvent::KeyEvent;
    std::string to_string() const override { std::stringstream out_stream; out_stream << get_name() << ": KeyCode=" << (Input::InputCode)m_keycode; return out_stream.str();}
};


////    MOUSE EVENTS

class MouseMovedEvent : public Event
{
public:
    EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse | EventCategoryAxis)
    EVENT_CLASS_TYPE(MouseMoved)

    MouseMovedEvent(double x, double y) : m_x(x), m_y(y) {}

    std::string to_string() const override { std::stringstream out_stream; out_stream << get_name() << ": X=" << m_x << ", Y=" << m_y; return out_stream.str();}

    double get_x() { return m_x; }
    double get_y() { return m_y; }

private:
    double m_x;
    double m_y;
};

class MouseEnterEvent : public Event
{
public:
    EVENT_CLASS_CATEGORY(EventCategoryMouse)
    EVENT_CLASS_TYPE(MouseEnter)
};

class MouseExitEvent : public Event
{
public:
    EVENT_CLASS_CATEGORY(EventCategoryMouse)
    EVENT_CLASS_TYPE(MouseExit)
};

/**
 * Abstract class for mouse button input events.
 * 
 */
class MouseBtnEvent : public Event
{
public:
    EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse | EventCategoryButton)
    MouseBtnEvent(int button) : m_button(static_cast<Input::MouseButton>(button)) {}
    virtual ~MouseBtnEvent() {};

    Input::MouseButton get_button() { return m_button; }

protected:
    Input::MouseButton m_button;
};

class MouseBtnPressedEvent : public MouseBtnEvent
{
public:
    EVENT_CLASS_TYPE(MouseBtnPressed)

    using MouseBtnEvent::MouseBtnEvent;

    std::string to_string() const override { std::stringstream out_stream; out_stream << get_name() << ": Button=" << (Input::InputCode)m_button; return out_stream.str();}
};

class MouseBtnReleasedEvent : public MouseBtnEvent
{
public:
    EVENT_CLASS_TYPE(MouseBtnReleased)

    using MouseBtnEvent::MouseBtnEvent;

    std::string to_string() const override { std::stringstream out_stream; out_stream << get_name() << ": Button=" << (Input::InputCode)m_button; return out_stream.str();}
};

class MouseScrolledEvent : public Event
{
public:
    EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse | EventCategoryAxis)
    EVENT_CLASS_TYPE(MouseScrolled)

    MouseScrolledEvent(double scroll_amt) : m_scroll_amt(scroll_amt) {}

    double get_scroll_amt() { return m_scroll_amt; }

    std::string to_string() const override { std::stringstream out_stream; out_stream << get_name() << ": Ammount=" << m_scroll_amt; return out_stream.str();}

private:

    /**
     * Ammount scrolled in this event.
     * 
     * Note that this event only has a single value.
     * GLFW exposes an x and y scroll ammount, but most mice only use the y.
     * Could look into implementing x scroll in future
     */
    double m_scroll_amt;
};


}