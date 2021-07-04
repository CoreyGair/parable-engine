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
    void on_update();
    void on_event(Event* e);

    bool enabled;

private:
    std::string m_name;
    std::vector<Control> m_controls;
};


}