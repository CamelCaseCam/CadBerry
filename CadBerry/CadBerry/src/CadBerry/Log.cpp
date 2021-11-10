#include "cdbpch.h"
#include "Log.h"

namespace CDB
{
	std::shared_ptr<spdlog::logger> Log::EditorLogger;
	std::shared_ptr<spdlog::logger> Log::BuildLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[Thread %t, %T] %n: %v%$");
		EditorLogger = spdlog::stdout_color_mt("Editor");
		EditorLogger->set_level(spdlog::level::trace);

		BuildLogger = spdlog::stdout_color_mt("Build");
		BuildLogger->set_level(spdlog::level::trace);
	}

	void Log::WriteInfo(std::string info)
	{
		CDB_EditorInfo(info);
	}
}