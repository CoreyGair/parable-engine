#pragma once

#include "pblpch.h"

#include "Core/Base.h"
#include "Events/InputEvent.h"
#include "Input/InputCodes.h"

#include <glm/glm.hpp>

namespace Parable::Input
{

#define AXIS_ID(ID) static AxisID get_static_axis_ID() { return AxisID::ID; }\
					virtual AxisID get_axis_ID() const override { return get_static_axis_ID(); }\


enum class AxisID  : unsigned short int
{
    MouseX=0,MouseY=1,MouseScroll=2,
    Button=3
};

// fn to decide return type of axis ID
// must update if change values of AxisID enum
#define IS_AXIS_DOUBLE(ID) (ID == 0 || ID == 1 || ID == 2)
#define IS_AXIS_INT(ID) (ID == 3)

template<class T>
class Axis
{
public:
    Axis(T initial_val) : m_value(initial_val) {}
    virtual void reset() { m_dirty = false; };
    virtual void on_event(Event* e) = 0;
    virtual T get_value() { return m_value; }
	virtual AxisID get_axis_ID() const = 0;

    bool dirty() { return m_dirty; }

protected:
    T m_value;
    // set if this axis has handled an event
    bool m_dirty = false;
};

class MouseXAxis : public Axis<double>
{
public:
    MouseXAxis() : Axis(0.0) {}
    void on_event(Event* e) override;
    void reset() { m_value = 0.0; Axis::reset(); }
    AXIS_ID(MouseX)
private:
    bool event_to_value(MouseMovedEvent& e);
};

class MouseYAxis : public Axis<double>
{
public:
    MouseYAxis() : Axis(0.0) {}
    void on_event(Event* e) override;
    void reset() { m_value = 0.0; Axis::reset(); }
    AXIS_ID(MouseY)
private:
    bool event_to_value(MouseMovedEvent& e);
};

class MouseScrollAxis : public Axis<double>
{
public:
    MouseScrollAxis() : Axis(0.0) {}
    void on_event(Event* e) override;
    void reset() { m_value = 0; Axis::reset(); }
    AXIS_ID(MouseScroll)
private:
    bool event_to_value(MouseScrolledEvent& e);
};

class ButtonAxis : public Axis<int>
{
public:
    ButtonAxis(std::vector<InputCode>& positive_inputs, std::vector<InputCode>& negative_inputs) : m_positive_inputs(std::move(positive_inputs)), m_negative_inputs(std::move(negative_inputs)), Axis(0.0) {}
    ButtonAxis(std::vector<InputCode>& positive_inputs) : ButtonAxis(std::move(positive_inputs), std::vector<InputCode>()) {}

    void reset() { Axis::reset(); }

    int get_value() override { update_value(); return m_value; }

    void on_event(Event* e) override;
    AXIS_ID(Button)

private:
    std::vector<InputCode> m_positive_inputs;
    std::vector<InputCode> m_negative_inputs;

    unsigned short int m_num_positive_down = 0;
    unsigned short int m_num_negative_down = 0;

    bool is_positive_input(InputCode c);
    bool is_negative_input(InputCode c);

    bool handle_button_pressed(InputCode c);
    bool handle_button_released(InputCode c);

    bool handle_key_pressed(KeyPressedEvent& e);
    bool handle_mousebtn_pressed(MouseBtnPressedEvent& e);
    bool handle_key_released(KeyReleasedEvent& e);
    bool handle_mousebtn_released(MouseBtnReleasedEvent& e);

    void update_value();

};


}