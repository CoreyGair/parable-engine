#pragma once

#include "Core/Base.h"
#include "Core/Layer.h"

#include "Input/InputManager.h"


namespace Parable::Input
{

/**
 * Engine layer responsible for the Input system
 * 
 */
class InputLayer : public Layer
{
public:
    InputLayer() : Layer("InputLayer"), m_input_manager() {}

    void on_update() override;
    void on_event(Event* e) override;

private:
    InputManager m_input_manager;
};


}