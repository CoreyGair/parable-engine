#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include "Events/Event.h"

namespace Parable
{

// layers represent different parts/modules of the engine,
// and are accumulated in a stack managed by the Application class
class Layer
{
private:
    std::string m_name;

public:
    Layer(const std::string& name = "Layer");
    virtual ~Layer();

    virtual bool on_event(EventDispatcher& dipatcher);
};


}