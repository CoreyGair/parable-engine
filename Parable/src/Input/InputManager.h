#pragma once

#include "Core/Base.h"

#include "Input/InputCodes.h"

namespace Parable::Input
{

// define flags for key state
#define BTN_HELD BIT(0)     // key is currently down
#define BTN_PRESSED BIT(1)  // key was pressed this frame
using ButtonState = unsigned short int;


class InputManager
{
public:
    void on_event(EventDispatcher& dispatcher);
private:
    std::map<KeyCode,ButtonState> m_key_states;
    std::map<MouseBtnCode,ButtonState> m_mousebtn_states;

    
}


}