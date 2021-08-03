#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include "Events/Event.h"

namespace Parable
{

/**
 * Abstract base for engine layers.
 * 
 * Layers represent different parts/modules of the engine,
 * and are accumulated in a stack managed by the Application class
 */
class Layer
{
private:
    std::string m_name;

public:
    Layer(const std::string& name = "Layer") : m_name(name) {}
    virtual ~Layer() {}

    /**
     * Run once each frame
     * 
     */
    virtual void on_update() = 0;
    /**
     * Processes an event.
     * 
     * @param e 
     */
    virtual void on_event(Event* e) = 0;
};


}