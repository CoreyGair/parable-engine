#pragma once

#include "Core/Base.h"
#include "Core/Layer.h"

#include "Input/InputManager.h"


namespace Parable::Input
{


class InputLayer : public Layer
{
public:
    InputLayer() : Layer("InputLayer"), m_input_manager(new InputManager()) {}

    void on_update() override;
    void on_event(Event* e) override;

private:
    std::unique_ptr<InputManager> m_input_manager;
};


}