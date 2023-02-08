#pragma once

#include <cdbpch.h>
#include "Core.h"

#pragma warning(push, 0)
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"
#pragma warning(pop)

#include <memory>

namespace CDB
{
	class CDBAPI Log
	{
	private:
		static std::shared_ptr<spdlog::logger> EditorLogger;
		static std::shared_ptr<spdlog::logger> BuildLogger;

	public:
		static void Init();
		static void DeInit();

		static spdlog::logger* GetEditorLogger() { return EditorLogger.get(); }
		static spdlog::logger* GetBuildLogger() { return BuildLogger.get(); }

		static void WriteInfo(std::string info);
	};
}

#define CDB_EditorFatal(...) ::CDB::Log::GetEditorLogger()->critical(__VA_ARGS__)
#define CDB_EditorError(...) ::CDB::Log::GetEditorLogger()->error(__VA_ARGS__)
#define CDB_EditorWarning(...) ::CDB::Log::GetEditorLogger()->warn(__VA_ARGS__)
#define CDB_EditorInfo(...) ::CDB::Log::GetEditorLogger()->info(__VA_ARGS__)
#define CDB_EditorTrace(...) ::CDB::Log::GetEditorLogger()->trace(__VA_ARGS__)

#define CDB_BuildFatal(...) ::CDB::Log::GetBuildLogger()->critical(__VA_ARGS__)
#define CDB_BuildError(...) ::CDB::Log::GetBuildLogger()->error(__VA_ARGS__)
#define CDB_BuildWarning(...) ::CDB::Log::GetBuildLogger()->warn(__VA_ARGS__)
#define CDB_BuildInfo(...) ::CDB::Log::GetBuildLogger()->info(__VA_ARGS__)
#define CDB_BuildTrace(...) ::CDB::Log::GetBuildLogger()->trace(__VA_ARGS__)