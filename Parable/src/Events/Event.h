#pragma once

#include "pblpch.h"

#include "Base.h"

namespace Parable{


enum class EventType 
{
    None = 0
};

enum EventCategory
{
    None = 0
};


class Event
{

public:
    virtual ~Event();

    bool Handled = false;

	virtual EventType GetEventType() const = 0;
	virtual const char* GetName() const = 0;
	virtual int GetCategoryFlags() const = 0;
	virtual std::string ToString() const { return GetName(); }

	bool IsInCategory(EventCategory category)
	{
		return GetCategoryFlags() & category;
	}

}

}