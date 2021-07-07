#pragma once

#include "pblpch.h"

#include "Core/Base.h"
#include "Events/InputEvent.h"
#include "Input/InputCodes.h"
#include "Input/Axis.h"

#include <glm/glm.hpp>

namespace Parable::Input
{

enum ControlType : unsigned short int
{
    Button=0,
    Axis1D=1,
    Axis2D=2,
    Axis3D=3,
    Axis1DInt=4,
    Axis2DInt=5,
    Axis3DInt=6
};


#define INPUT_CONTROL_TYPE(control_type) public: static ControlType get_static_control_type() { return ControlType::control_type; }\
                                                ControlType get_control_type() const override { return get_static_control_type(); }

// method declarations
#define AXIS_METHODS public: void on_update() override;\
                                void on_event(Event* e) override;\
                                void notify_changed() { private_notify_changed(m_cached_value); }\
                                return_t& get_value() { return m_cached_value; }\
                                protected: return_t m_cached_value;

class Control
{
public:
    Control(std::string& name) : m_name(name) {}
    virtual ~Control() = default;
    virtual void on_update() = 0;
    virtual void on_event(Event* e) = 0;

    virtual ControlType get_control_type() const = 0;
    
    const std::string& get_name() { return m_name; }

protected:
    std::string m_name;
};

using ButtonControlCallback = std::function<void(void)>;

// is really just a button axis wrapper
class ButtonControl : public Control
{
    INPUT_CONTROL_TYPE(Button)
public:
    ButtonControl(std::string& name, std::vector<InputCode>& buttons) : Control(name), m_button_axis(std::move(buttons)) {}

    void on_update() override;
    void on_event(Event* e) override;

    bool pressed_this_frame() { return m_pressed_this_frame; }
    bool released_this_frame() { return m_released_this_frame; }

    void notify_pressed() { for(auto it = m_pressed_callbacks.cbegin(); it != m_pressed_callbacks.cend(); ++it) (*it)(); }
    void notify_released() { for(auto it = m_released_callbacks.cbegin(); it != m_released_callbacks.cend(); ++it) (*it)(); }

    bool& get_value() { return m_cached_value; }

private:
    bool m_cached_value;

    ButtonAxis m_button_axis;

    bool m_pressed_this_frame;
    bool m_released_this_frame;

    std::vector<ButtonControlCallback> m_pressed_callbacks;
    std::vector<ButtonControlCallback> m_released_callbacks;
};

template<class T>
using AxisControlCallback = std::function<void(T)>;

template<class T>
class AxisControl : public Control
{
    using Control::Control;
public:
    void changed_this_frame() { return m_changed_this_frame};

    virtual void notify_changed() = 0;
protected:
    bool m_changed_this_frame;

    void private_notify_changed(T val) { for(auto it = m_changed_callbacks.cbegin(); it != m_changed_callbacks.cend(); ++it) (*it)(val); }
    std::vector<AxisControlCallback<T>> m_changed_callbacks;
};

template<class return_t>
class Axis1DControlBase : public AxisControl<return_t>
{
    AXIS_METHODS
public:
    Axis1DControlBase(std::string& name, std::unique_ptr<Axis<return_t>>& axis) : AxisControl(name), m_axis(std::move(axis)) {}

protected:
    std::unique_ptr<Axis<return_t>> m_axis;
};

class Axis1DControl : public Axis1DControlBase<double>
{
    // could remove this ugly template arg with an extra class and typedef but this works for now
    using Axis1DControlBase<double>::Axis1DControlBase;
    INPUT_CONTROL_TYPE(Axis1D)
};

class Axis1DIntControl : public Axis1DControlBase<int>
{
    using Axis1DControlBase<int>::Axis1DControlBase;
    INPUT_CONTROL_TYPE(Axis1DInt)
};

template<class return_t, class axis_t>
class Axis2DControlBase : public AxisControl<return_t>
{
    AXIS_METHODS
public:
    Axis2DControlBase(std::string& name, std::unique_ptr<Axis<axis_t>>& x_axis,
                                            std::unique_ptr<Axis<axis_t>>& y_axis) : AxisControl(name), 
                                                                                        m_xaxis(std::move(x_axis)),
                                                                                        m_yaxis(std::move(y_axis)) {}

protected:
    std::unique_ptr<Axis<axis_t>> m_xaxis;
    std::unique_ptr<Axis<axis_t>> m_yaxis;
};

class Axis2DControl : public Axis2DControlBase<glm::vec2,double>
{
    using Axis2DControlBase<glm::vec2,double>::Axis2DControlBase;
    INPUT_CONTROL_TYPE(Axis2D)
};

class Axis2DIntControl : public Axis2DControlBase<glm::ivec2,int>
{
    using Axis2DControlBase<glm::ivec2,int>::Axis2DControlBase;
    INPUT_CONTROL_TYPE(Axis2DInt)
};

template<class return_t, class axis_t>
class Axis3DControlBase : public AxisControl<return_t>
{
    AXIS_METHODS
public:
    Axis3DControlBase(std::string& name, std::unique_ptr<Axis<axis_t>>& x_axis,
                                            std::unique_ptr<Axis<axis_t>>& y_axis,
                                            std::unique_ptr<Axis<axis_t>>& z_axis) : AxisControl(name), 
                                                                                        m_xaxis(std::move(x_axis)),
                                                                                        m_yaxis(std::move(y_axis)),
                                                                                        m_zaxis(std::move(z_axis)) {}

protected:
    std::unique_ptr<Axis<axis_t>> m_xaxis;
    std::unique_ptr<Axis<axis_t>> m_yaxis;
    std::unique_ptr<Axis<axis_t>> m_zaxis;
};

class Axis3DControl : public Axis3DControlBase<glm::vec3,double>
{
    using Axis3DControlBase<glm::vec3,double>::Axis3DControlBase;
    INPUT_CONTROL_TYPE(Axis3D)
};

class Axis3DIntControl : public Axis3DControlBase<glm::ivec3,int>
{
    using Axis3DControlBase<glm::ivec3,int>::Axis3DControlBase;
    INPUT_CONTROL_TYPE(Axis3DInt)
};


}

// just template things...
#include "Control.tpp"