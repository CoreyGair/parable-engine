#pragma once

#include "pblpch.h"

#include "Core/Base.h"
#include "Events/InputEvent.h"
#include "Input/InputCodes.h"

#include <glm/glm.hpp>

namespace Parable::Input
{

#define AXIS_ID(ID) static AxisID get_static_axis_ID() { return ID; }\
					virtual AxisID get_axis_ID() const override { return get_static_axis_ID(); }\

using AxisID = unsigned short int;
enum : AxisID
{
    None=0,
    MouseX,MouseY,MouseScroll,
    Button
};

template<class T>
class Axis
{
public:
    virtual ~Axis() = 0;
    virtual void reset() { m_dirty = false; };
    virtual void on_event(Event* e) = 0;
    T get_value() { return m_value; }
	virtual AxisID get_axis_ID() const = 0;

    bool dirty() { return m_dirty; }

protected:
    T m_value;
    // set if this axis has handled an event
    bool m_dirty;
};

class MouseXAxis : public Axis<double>
{
public:
    void on_event(Event* e) override;
    void reset() { m_value = 0.0; Axis::reset(); }
    AXIS_ID(MouseX)
private:
    bool event_to_value(MouseMovedEvent& e);
};

class MouseYAxis : public Axis<double>
{
public:
    void on_event(Event* e) override;
    void reset() { m_value = 0.0; Axis::reset(); }
    AXIS_ID(MouseY)
private:
    bool event_to_value(MouseMovedEvent& e);
};

class MouseScrollAxis : public Axis<int>
{
public:
    void on_event(Event* e) override;
    void reset() { m_value = 0; Axis::reset(); }
    AXIS_ID(MouseScroll)
private:
    bool event_to_value(MouseScrolledEvent& e);
};

class ButtonAxis : public Axis<int>
{
public:
    ButtonAxis(InputCode positive, InputCode negative) : m_positive_input(positive), m_negative_input(negative) {}

    void reset() { Axis::reset(); }

    void on_event(Event* e) override;
    AXIS_ID(Button)

private:
    InputCode m_positive_input;
    InputCode m_negative_input;

    bool handle_button_pressed(InputCode c);
    bool handle_button_released(InputCode c);

    bool handle_key_pressed(KeyPressedEvent& e);
    bool handle_mousebtn_pressed(MouseBtnPressedEvent& e);
    bool handle_key_released(KeyReleasedEvent& e);
    bool handle_mousebtn_released(MouseBtnReleasedEvent& e);

};


}