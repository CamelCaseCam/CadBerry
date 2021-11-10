#pragma once
#include <cdbpch.h>
#include "Event.h"

namespace CDB
{
	//Event called when a user starts a build before any part of the build process is started. This lets you save the user's work before a 
	//build
	class CDBAPI PrebuildEvent : public Event
	{
	public:
		std::string ToString() const override
		{
			return std::string("PrebuildEvent");
		}

		EVENT_CLASS_TYPE(Prebuild)
		EVENT_CLASS_CATEGORY(EventCategoryBuild | EventCategoryApplication)
	};

	//Called at the beginning of the build, after PrebuildEvent
	class CDBAPI BuildStartEvent : public Event
	{
	public:
		std::string ToString() const override
		{
			return std::string("BuildStartEvent");
		}

		EVENT_CLASS_TYPE(BuildStart)
		EVENT_CLASS_CATEGORY(EventCategoryBuild | EventCategoryApplication)
	};

	//Called after build is completed
	class CDBAPI PostbuildEvent : public Event
	{
	public:
		std::string ToString() const override
		{
			return std::string("PostbuildEvent");
		}

		EVENT_CLASS_TYPE(Postbuild)
		EVENT_CLASS_CATEGORY(EventCategoryBuild | EventCategoryApplication)
	};
}