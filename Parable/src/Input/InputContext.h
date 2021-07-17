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
    InputContext(std::string& name, std::vector<ButtonMap>& button_maps) : m_name(std::move(name)), m_button_maps(std::move(button_maps)) {}
    InputContext(InputContext&& other) : InputContext(other.m_name, other.m_button_maps) {}

    void on_update();
    void on_input_pressed(InputCode code);
    void on_input_released(InputCode code);

    bool enabled = true;

private:
    std::string m_name;
    std::vector<ButtonMap> m_button_maps;
};


}