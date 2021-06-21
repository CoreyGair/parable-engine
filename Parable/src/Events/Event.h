#pragma once

#include "pblpch.h"

#include "Base.h"

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

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

class Event
{

public:
	typedef std::unique_ptr<Event> EventUPtr;

    virtual ~Event();

    bool handled = false;

	virtual EventType GetEventType() const = 0;
	virtual const char* GetName() const = 0;
	virtual int GetEventCategory() const = 0;
	virtual std::string ToString() const { return GetName(); }

	bool IsInCategory(EventCategory category)
	{
		return GetEventCategory() & category;
	}

};

class EventDispatcher
{
public:
	EventDispatcher(Event::EventUPtr event) { m_event = std::move(event); }

	bool handled() { return m_event->handled; }

	// dspatches event to handler func
	// sets event.handled if event was handled
	// returns true if the correct event type was passed (so if func was called)
	template<typename T>
	bool Dispatch(const std::function<bool(T&)>& func)
	{
		if (m_event.GetEventType() == T::GetStaticType())
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