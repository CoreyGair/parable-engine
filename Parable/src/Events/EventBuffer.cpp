#include "Events/EventBuffer.h"


namespace Parable
{


/**
 * Pop the next queued event
 *
 * @return the unique ptr to the next event
 */
Event::EventUPtr EventBuffer::next()
{
    Event::EventUPtr e = std::move(m_event_queue.front());
    m_event_queue.pop();
    return e;
}


}