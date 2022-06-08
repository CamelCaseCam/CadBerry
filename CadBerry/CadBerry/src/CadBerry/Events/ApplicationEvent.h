#pragma once
#include <cdbpch.h>
#include "Event.h"

namespace CDB
{
	class CDBAPI WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(int width, int height) : NewWidth(width), NewHeight(height) {}

		inline int GetWidth() { return NewWidth; }
		inline int GetHeight() { return NewHeight; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << NewWidth << " by " << NewHeight;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		int NewWidth, NewHeight;
	};

	class CDBAPI WindowCloseEvent : public Event
	{
	public:
		std::string ToString() const override
		{
			return "WindowCloseEvent";
		}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	//Called when the application starts
	class CDBAPI StartEvent : public Event
	{
	public:
		std::string ToString() const override
		{
			return std::string("StartEvent");
		}

		EVENT_CLASS_TYPE(Start)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	//Will be removed
	class CDBAPI UpdateEvent : public Event
	{
	public:
		UpdateEvent(float time) : dt(time) {}

		inline float GetTime() const { return dt; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "UpdateEvent with dt " << dt;
			return ss.str();
		}

		EVENT_CLASS_TYPE(Update)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		float dt;
	};

	//Not yet implemented
	class CDBAPI PanelOpenedEvent : public Event
	{
	public:
		std::string ToString() const override
		{
			return std::string("PanelOpenedEvent");
		}

		EVENT_CLASS_TYPE(PanelOpened)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	//Not yet implemented
	class CDBAPI PanelClosedEvent : public Event
	{
	public:
		std::string ToString() const override
		{
			return std::string("PanelClosedEvent");
		}

		EVENT_CLASS_TYPE(PanelClosed)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}