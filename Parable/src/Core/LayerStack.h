#pragma once

#include "Events/Event.h"

#include <vector>

namespace Parable
{

class Layer;

class LayerStack
{
private:
    std::vector<Layer*> m_layers;

public:
    void push(Layer* layer) { m_layers.push_back(layer); }

    auto cbegin() { return m_layers.cbegin(); }
    auto cend() { return m_layers.cend(); }
};


}