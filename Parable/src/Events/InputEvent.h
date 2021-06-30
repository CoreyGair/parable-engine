#pragma once

#include "Core/Base.h"
#include "Events/Event.h"

#include "Core/KeyCode.h"

#include <sstream>

namespace Parable
{


class KeyEvent : public Event
{
public:
    EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard)

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
    std::string to_string() const override { std::stringstream out_stream; out_stream << "KeyPressedEvent: KeyCode=" << m_keycode; return out_stream.str();}
};

class KeyRepeatedEvent : public KeyEvent
{
public:
    EVENT_CLASS_TYPE(KeyRepeated)
    KeyRepeatedEvent(const KeyCode key) : KeyEvent(key) {}
    std::string to_string() const override { std::stringstream out_stream; out_stream << "KeyRepeatedEvent: KeyCode=" << m_keycode; return out_stream.str();}
};

class KeyReleasedEvent : public KeyEvent
{
public:
    EVENT_CLASS_TYPE(KeyReleased)
    KeyReleasedEvent(const KeyCode key) : KeyEvent(key) {}
    std::string to_string() const override { std::stringstream out_stream; out_stream << "KeyReleasedEvent: KeyCode=" << m_keycode; return out_stream.str();}
};


}