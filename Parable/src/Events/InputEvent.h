#pragma once

#include "Core/Base.h"
#include "Events/Event.h"

#include "Core/KeyCode.h"

#include <sstream>

namespace Parable
{


//// KEYBOARD EVENTS


class KeyEvent : public Event
{
public:
    EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard)
    virtual ~KeyEvent() = default;

    KeyCode get_key_code() { return m_keycode; }

protected:
    KeyEvent(const KeyCode key) : m_keycode(key) {}
    KeyCode m_keycode;
};


class KeyPressedEvent : public KeyEvent
{
public:
    EVENT_CLASS_TYPE(KeyPressed)
    KeyPressedEvent(const KeyCode key) : KeyEvent(key) {}
    std::string to_string() const override { std::stringstream out_stream; out_stream << get_name() << ": KeyCode=" << m_keycode; return out_stream.str();}
};

class KeyRepeatedEvent : public KeyEvent
{
public:
    EVENT_CLASS_TYPE(KeyRepeated)
    KeyRepeatedEvent(const KeyCode key) : KeyEvent(key) {}
    std::string to_string() const override { std::stringstream out_stream; out_stream << get_name() << ": KeyCode=" << m_keycode; return out_stream.str();}
};

class KeyReleasedEvent : public KeyEvent
{
public:
    EVENT_CLASS_TYPE(KeyReleased)
    KeyReleasedEvent(const KeyCode key) : KeyEvent(key) {}
    std::string to_string() const override { std::stringstream out_stream; out_stream << get_name() << ": KeyCode=" << m_keycode; return out_stream.str();}
};


////    MOUSE EVENTS


class MouseEvent : public Event
{
public:
    EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)
    virtual ~MouseEvent() = default;
};

class MouseMovedEvent : public MouseEvent
{
public:
    EVENT_CLASS_TYPE(MouseMoved)

    MouseMovedEvent(double x, double y) : m_x(x), m_y(y) {}

    std::string to_string() const override { std::stringstream out_stream; out_stream << get_name() << ": X=" << m_x << ", Y=" << m_y; return out_stream.str();}

    double get_x() { return m_x; }
    double get_y() { return m_y; }

private:
    double m_x;
    double m_y;
};

class MouseEnterEvent : public MouseEvent
{
public:
    EVENT_CLASS_TYPE(MouseEnter)
};

class MouseExitEvent : public MouseEvent
{
public:
    EVENT_CLASS_TYPE(MouseExit)
};

class MouseBtnEvent : public MouseEvent
{
public:
    MouseBtnEvent(int button) : m_button(button) {}
    virtual ~MouseBtnEvent() {};

    MouseCode get_button() { return m_button; }

protected:
    MouseCode m_button;
};

class MouseBtnPressedEvent : public MouseBtnEvent
{
public:
    EVENT_CLASS_TYPE(MouseBtnPressed)

    MouseBtnPressedEvent(int button) : MouseBtnEvent(button) {}

    std::string to_string() const override { std::stringstream out_stream; out_stream << get_name() << ": Button=" << m_button; return out_stream.str();}
};

class MouseBtnReleasedEvent : public MouseBtnEvent
{
public:
    EVENT_CLASS_TYPE(MouseBtnReleased)

    MouseBtnReleasedEvent(int button) : MouseBtnEvent(button) {}

    std::string to_string() const override { std::stringstream out_stream; out_stream << get_name() << ": Button=" << m_button; return out_stream.str();}
};

class MouseScrolledEvent : public MouseEvent
{
public:
    EVENT_CLASS_TYPE(MouseScrolled)

    MouseScrolledEvent(int scroll_amt) : m_scroll_amt(scroll_amt) {}

    std::string to_string() const override { std::stringstream out_stream; out_stream << get_name() << ": Ammount=" << m_scroll_amt; return out_stream.str();}

private:
    // note: the event only has a single scroll ammount value
    // glfw exposes both an x and y scroll amt; most normal mice emit y ammounts so this is what we use
    // may look into when the x scroll is uses in future and implement it
    int m_scroll_amt;
};


}