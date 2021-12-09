#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include <functional>

namespace Parable{


/**
 * Describes the specific type of engine events.
 * 
 */
enum class EventType 
{
    None = 0,
	WindowClose,WindowResize,
	KeyPressed,KeyRepeated,KeyReleased,
	MouseMoved,MouseEnter,MouseExit,MouseBtnPressed,MouseBtnReleased,MouseScrolled
};

/**
 * Describes categories for filtering engine events.
 */
enum EventCategory
{
    None = 0,
	EventCategoryWindow = BIT(0),
	EventCategoryInput = BIT(1),
	EventCategoryKeyboard = BIT(2),
	EventCategoryMouse = BIT(3),
	// used by input system 
	EventCategoryButton = BIT(4),
	EventCategoryAxis = BIT(5)
};

#define EVENT_CLASS_TYPE(type) static EventType get_static_type() { return EventType::type; }\
								virtual EventType get_event_type() const override { return get_static_type(); }\
								virtual const char* get_name() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int get_event_category() const override { return category; }


/**
 * Abstract class for engine events.
 * 
 * Parable has a main event backbone, which passs data from the application to the engine layers.
 * 
 */
class Event
{

public:
	typedef std::unique_ptr<Event> EventUPtr;

    virtual ~Event() = default;

    bool handled = false;

	virtual EventType get_event_type() const = 0;
	virtual const char* get_name() const = 0;
	virtual int get_event_category() const = 0;
	virtual std::string to_string() const { return get_name(); }

	bool in_category(EventCategory category)
	{
		return get_event_category() & category;
	}

};


/**
 * Dispatches events to engine layers.
 */
class EventDispatcher
{
public:
	EventDispatcher(Event* event) : m_event(event) {} 

	/**
	 * Dispatches m_event to a handler function.
	 * 
	 * Sets m_event.handled if the handler returns true (the event was handled),
	 * and returns true if the correct event type was passed (and so the handler was invoked).
	 * 
	 * @tparam T the concrete event type the handler accepts
	 * 
	 * @param func function pointer to the handler
	 */
	template<typename T>
	bool dispatch(const std::function<bool(T&)>& func)
	{
		if (m_event->get_event_type() == T::get_static_type())
		{
			m_event->handled |= func(static_cast<T&>(*m_event));
			return true;
		}
		return false;
	}

	/**
	 * Dispatch a pointer to the event.
	 * 
	 * Use carefully, mostly deprecated but used for some debugging for now.
	 * 
	 * @param func function pointer to handlers
	 */
	bool dispatch_raw(const std::function<bool(Event*)>& func)
	{
		m_event->handled |= func(m_event);
		return true;
	}

private:
	/**
	 * The event which we are dispatching.
	 * 
	 */
	Event* m_event;
};


}