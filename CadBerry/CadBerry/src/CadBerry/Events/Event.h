#pragma once
#include <cdbpch.h>
#include "CadBerry/Core.h"

#include "spdlog/fmt/ostr.h"

namespace CDB
{
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize,
		Start, Update, PanelOpened, PanelClosed,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
		Prebuild, BuildStart, Postbuild    //Mostly for the build engine
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4),
		EventCategoryBuild = BIT(5)
	};

//Some macros
#ifdef CDB_PLATFORM_WINDOWS
	#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }
#else
	#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
							   virtual EventType GetEventType() const override { return GetStaticType(); }\
							   virtual const char* GetName() const override { return #type; }
#endif

#define	EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	class CDBAPI Event
	{
		friend class EventDispatcher;
	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool Handled() const { return m_Handled; }

		inline bool IsInCategory(EventCategory Category)
		{
			return GetCategoryFlags() & Category;
		}

		bool m_Handled = false;
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& evnt) : m_Event(evnt) {}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}