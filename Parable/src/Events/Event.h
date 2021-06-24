#pragma once

#include "pblpch.h"

#include "Core/Base.h"

#include <functional>

namespace Parable{


enum class EventType 
{
    None = 0
};

enum EventCategory
{
    None = 0
};

#define EVENT_CLASS_TYPE(type) static EventType get_static_type() { return EventType::type; }\
								virtual EventType get_event_type() const override { return get_static_type(); }\
								virtual const char* get_name() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int get_event_category() const override { return category; }

class Event
{

public:
	typedef std::unique_ptr<Event> EventUPtr;

    virtual ~Event();

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

class EventDispatcher
{
public:
	EventDispatcher(Event::EventUPtr event) : m_event(std::move(event)) {} 

	bool handled() { return m_event->handled; }

	// dspatches event to handler func
	// sets event.handled if event was handled
	// returns true if the correct event type was passed (so if func was called)
	template<typename T>
	bool Dispatch(const std::function<bool(T&)>& func)
	{
		if (m_event.get_event_type() == T::get_static_type())
		{
			m_event->handled |= func(static_cast<T&>(*m_event));
			return true;
		}
		return false;
	}

private:
	Event::EventUPtr m_event;
};

}