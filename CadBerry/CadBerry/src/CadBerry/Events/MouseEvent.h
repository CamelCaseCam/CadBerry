#pragma once
#include <cdbpch.h>
#include "Event.h"

namespace CDB
{
	class CDBAPI MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y) : MouseX(x), MouseY(y) {}

		inline float GetX() { return MouseX; }
		inline float GetY() { return MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent at pos (" << MouseX << "," << MouseY << ")";
			return ss.str();
		}
		EVENT_CLASS_TYPE(MouseMoved)

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float MouseX, MouseY;
	};

	class CDBAPI MouseButtonEvent : public Event
	{
	public:
		inline int GetButtonCode() { return ButtonCode; }

		inline float GetX() { return MouseX; }
		inline float GetY() { return MouseY; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryMouseButton | EventCategoryInput)
	protected:
		MouseButtonEvent(float x, float y, int button) : MouseX(x), MouseY(y), ButtonCode(button) {}
		int ButtonCode;
		float MouseX, MouseY;
	};

	class CDBAPI MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float x, float y) : XOffset(x), YOffset(y) {}

		inline float GetXOffset() const { return XOffset; }
		inline float GetYOffset() const { return YOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << XOffset << ", " << YOffset;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float XOffset, YOffset;
	};

	class CDBAPI MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(float x, float y, int button) : MouseButtonEvent(x, y, button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent at pos (" << MouseX << "," << MouseY << ")";
			return ss.str();
		}
		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class CDBAPI MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(float x, float y, int button) : MouseButtonEvent(x, y, button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent at pos (" << MouseX << ", " << MouseY << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}