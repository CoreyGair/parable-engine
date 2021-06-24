#include "EventBuffer.h"


namespace Parable
{


// Return the unique_ptr to the front of the event queue, and pop it
Event::EventUPtr EventBuffer::next()
{
    Event::EventUPtr e = std::move(m_event_queue.front());
    m_event_queue.pop();
    return e;
}


}