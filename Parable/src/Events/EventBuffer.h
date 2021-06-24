#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include "Events/Event.h"

#include <queue>

namespace Parable{

class EventBuffer
{
private:
    std::queue<Event::EventUPtr> m_event_queue;
    bool front_in_use = false;

public:
    bool is_empty() { return m_event_queue.size() == 0; }
    void push(Event::EventUPtr e) { m_event_queue.push(std::move(e)); }
    Event::EventUPtr next();
};


}