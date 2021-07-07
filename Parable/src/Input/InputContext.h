#include "pblpch.h"

#include "Core/Base.h"
#include "Events/InputEvent.h"
#include "Input/InputCodes.h"
#include "Input/Axis.h"
#include "Input/Control.h"


#include <glm/glm.hpp>

namespace Parable::Input
{


class InputContext
{
public:
    InputContext(std::string& name, std::vector<std::unique_ptr<Control>>& controls) : m_name(std::move(name)), m_controls(std::move(controls)) {}
    InputContext(InputContext&& other) : InputContext(other.m_name, other.m_controls) {}

    void on_update();
    void on_event(Event* e);

    bool enabled;

private:
    std::string m_name;
    std::vector<std::unique_ptr<Control>> m_controls;
};


}