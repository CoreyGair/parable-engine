#include "EventBuffer.h"


namespace Parable
{


// Get the front of the event queue
// keeps the Event object alive until next item requested
// when next is called, it is assumed the caller is done with the provious object, so it pops it
Event::EventUPtr EventBuffer::next()
{
    Event::EventUPtr e = std::move(m_event_queue.front());
    m_event_queue.pop();
    return e;
}


}