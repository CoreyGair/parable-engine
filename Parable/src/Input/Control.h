#include "pblpch.h"

#include "Core/Base.h"
#include "Events/InputEvent.h"
#include "Input/InputCodes.h"
#include "Input/Axis.h"

#include <glm/glm.hpp>

namespace Parable::Input
{

#define INPUT_CONTROL_TYPE(type) public: type& get_value() { return m_cached_value; }\
                                    protected: type m_cached_value;

class Control
{
public:
    virtual ~Control() = default;
    virtual void on_update() = 0;
    virtual void on_event(Event* e) = 0;
    
    const std::string& get_name() { return m_name; }

protected:
    std::string m_name;
};

using ButtonControlCallback = std::function<void(void)>;

// is really just a button axis wrapper
class ButtonControl : public Control
{
    INPUT_CONTROL_TYPE(bool)
public:
    ButtonControl(InputCode button) : m_button_axis(button, 0) {}

    void on_update() override;
    void on_event(Event* e) override;

    bool pressed_this_frame() { return m_pressed_this_frame; }
    bool released_this_frame() { return m_released_this_frame; }

    void notify_pressed() { for(auto it = m_pressed_callbacks.cbegin(); it != m_pressed_callbacks.cend(); ++it) (*it)(); }
    void notify_released() { for(auto it = m_released_callbacks.cbegin(); it != m_released_callbacks.cend(); ++it) (*it)(); }

private:
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
    INPUT_CONTROL_TYPE(T)
public:
    void changed_this_frame() { return m_changed_this_frame};

    void notify_changed() { for(auto it = m_changed_callback.cbegin(); it != m_changed_callback.cend(); ++it) (*it)(m_cached_value); }
protected:
    bool m_changed_this_frame;

    std::vector<AxisControlCallback<T>> m_changed_callback;
};

class Axis1DControl : public AxisControl<double>
{
public:
    void on_update() override;
    void on_event(Event* e) override;
private:
    Axis<double>* m_axis;
};

class Axis2DControl : public AxisControl<glm::vec2>
{
public:
    void on_update() override;
    void on_event(Event* e) override;
private:
    Axis<double>* m_xaxis;
    Axis<double>* m_yaxis;
};

class Axis3DControl : public AxisControl<glm::vec3>
{
public:
    void on_update() override;
    void on_event(Event* e) override;
private:
    Axis<double>* m_xaxis;
    Axis<double>* m_yaxis;
    Axis<double>* m_zaxis;
};


}