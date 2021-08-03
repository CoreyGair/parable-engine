#pragma once

#include "Events/Event.h"

#include <vector>

namespace Parable
{

class Layer;


// TODO: should prob use unique ptrs here...
/**
 * Holds layers in a stack for use by the Application.
 * 
 * Allows forwards iteration for update(), and reverse iteration for event()
 * 
 */
class LayerStack
{
private:
    std::vector<Layer*> m_layers;

public:
    void push(Layer* layer) { m_layers.push_back(layer); }

    auto cbegin() { return m_layers.cbegin(); }
    auto cend() { return m_layers.cend(); }
    auto crbegin() { return m_layers.crbegin(); }
    auto crend() { return m_layers.crend(); }
};


}