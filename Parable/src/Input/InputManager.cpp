#include "InputManager.h"


namespace Parable::Input
{


InputManager* InputManager::s_instance = nullptr;

InputManager::InputManager()
{
    PBL_CORE_ASSERT_MSG(!s_instance, "Input manager already exists!");
    s_instance = this;
}

void InputManager::on_update()
{
    for(auto& ctx : m_contexts)
    {
        if (ctx.enabled)
        {
            ctx.on_update();
        }
    }

    // 'reset pressed this frame' state
    m_input_state.key_pressed.reset();
    m_input_state.mouse_btn_pressed.reset();

    m_input_state.mouse_scroll_delta = 0.0;
    m_input_state.mouse_position_delta.x = 0; m_input_state.mouse_position_delta.y = 0;
}

void InputManager::on_event(Event* e)
{
    EventDispatcher dispatcher (e);
    dispatcher.dispatch<KeyPressedEvent>(PBL_BIND_MEMBER_EVENT_HANDLER(InputManager::key_pressed));
    dispatcher.dispatch<KeyReleasedEvent>(PBL_BIND_MEMBER_EVENT_HANDLER(InputManager::key_released));
    dispatcher.dispatch<MouseBtnPressedEvent>(PBL_BIND_MEMBER_EVENT_HANDLER(InputManager::mouse_btn_pressed));
    dispatcher.dispatch<MouseBtnReleasedEvent>(PBL_BIND_MEMBER_EVENT_HANDLER(InputManager::mouse_btn_released));
}

/**
 * Passes key pressed event to contexts and modifies polling state.
 *
 * Binds to event dispatcher.
 */
bool InputManager::key_pressed(KeyPressedEvent& e)
{
    InputCode key = (InputCode)e.get_key_code();
    m_input_state.key_down[key - (int)KeyCode::FIRST] = true;
    m_input_state.key_pressed[key - (int)KeyCode::FIRST] = true;
    notify_contexts_of_input_pressed(key);
    return true;
}

/**
 * Passes key released event to contexts and modifies polling state.
 *
 * Binds to event dispatcher.
 */
bool InputManager::key_released(KeyReleasedEvent& e)
{
    InputCode key = (InputCode)e.get_key_code();
    m_input_state.key_down[key - (int)KeyCode::FIRST] = false;
    notify_contexts_of_input_released(key);
    return true;
}

/**
 * Passes mouse button pressed event to contexts and modifies polling state.
 *
 * Binds to event dispatcher.
 */
bool InputManager::mouse_btn_pressed(MouseBtnPressedEvent& e)
{
    InputCode btn = (InputCode)e.get_button();
    m_input_state.mouse_btn_down[btn - (int)MouseButton::FIRST] = true;
    m_input_state.mouse_btn_pressed[btn - (int)MouseButton::FIRST] = true;
    notify_contexts_of_input_pressed(btn);
    return true;
}

/**
 * Passes mouse button released event to contexts and modifies polling state.
 *
 * Binds to event dispatcher.
 */
bool InputManager::mouse_btn_released(MouseBtnReleasedEvent& e)
{
    InputCode btn = (InputCode)e.get_button();
    m_input_state.mouse_btn_down[btn - (int)KeyCode::FIRST] = false;
    notify_contexts_of_input_released(btn);
    return true;
}

/**
 * Passes scroll event to contexts and modifies polling state.
 *
 * Binds to event dispatcher.
 */
bool InputManager::mouse_scrolled(MouseScrolledEvent& e)
{
    m_input_state.mouse_scroll_delta += e.get_scroll_amt(); return true;

    // allows us to use scroll as a button in button maps
    if (e.get_scroll_amt() > 0)
    {
        InputCode btn = (InputCode)MouseButton::MouseScrollUp;
        m_input_state.mouse_btn_pressed[btn - (int)MouseButton::FIRST] = true;
        notify_contexts_of_input_pressed(btn);
        notify_contexts_of_input_released(btn);
    }
    else if (e.get_scroll_amt() < 0)
    {
        InputCode btn = (InputCode)MouseButton::MouseScrollDown;
        m_input_state.mouse_btn_pressed[btn - (int)MouseButton::FIRST] = true;
        notify_contexts_of_input_pressed(btn);
        notify_contexts_of_input_released(btn);
    }
}

/**
 * Passes mouse moved event to contexts and modifies polling state.
 *
 * Binds to event dispatcher.
 */
bool InputManager::mouse_moved(MouseMovedEvent& e)
{
    glm::vec2 new_pos { e.get_x(), e.get_y() };
    m_input_state.mouse_position_delta += new_pos - m_input_state.mouse_position;
    m_input_state.mouse_position = std::move(new_pos);
    return true;
}

void InputManager::notify_contexts_of_input_pressed(InputCode code)
{
    for(auto& ctx : m_contexts)
    {
        if (ctx.enabled)
        {
            ctx.on_input_pressed(code);
        }
    }
}
void InputManager::notify_contexts_of_input_released(InputCode code)
{
    for(auto& ctx : m_contexts)
    {
        if (ctx.enabled)
        {
            ctx.on_input_released(code);
        }
    }
}



}