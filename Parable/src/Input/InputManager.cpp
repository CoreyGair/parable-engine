#include "InputManager.h"


// TEMP
#include "InputContextLoader.h"

namespace Parable::Input
{


InputManager* InputManager::s_instance = nullptr;

InputManager::InputManager()
{
    PBL_ASSERT_MSG(!s_instance, "Input manager already exists!");
    s_instance = this;
    load_context(std::string("context.json"));
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
}

void InputManager::on_event(Event* e)
{
    for(auto& ctx : m_contexts)
    {
        if (ctx.enabled)
        {
            ctx.on_event(e);
        }
    }
}

void InputManager::load_context(std::string context_json_file)
{
    InputContextLoader loader(context_json_file);
    m_contexts.push_back(std::move((*loader.load_context())));
}

}