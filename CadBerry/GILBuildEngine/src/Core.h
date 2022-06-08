#pragma once
#include <gilpch.h>

extern std::string DataPath;

#ifdef CDB_PLATFORM_WINDOWS
	#ifdef GIL_BUILD_DLL
		#define GILAPI __declspec(dllexport)
	#else
		#define GILAPI __declspec(dllimport)
	#endif
#else
#define GILAPI 
#endif

#define GIL_MAJOR 0
#define GIL_MINOR 2
#define GIL_FIX 0