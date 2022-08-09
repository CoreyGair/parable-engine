#pragma once

namespace Parable
{

class Window;

class Renderer
{
public:
    virtual void on_update() = 0;

    virtual void on_resize(unsigned int width, unsigned int height) = 0;

    void on_minimise(bool minimised) { m_paused = minimised; }

protected:
    bool m_paused = false;
};


}