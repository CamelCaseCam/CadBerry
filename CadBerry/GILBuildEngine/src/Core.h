#pragma once
#include <gilpch.h>

extern std::string DataPath;

#ifdef GIL_BUILD_DLL
	#define GILAPI __declspec(dllexport)
#else
	#define GILAPI __declspec(dllimport)
#endif

#define GIL_MAJOR 0
#define GIL_MINOR 2
#define GIL_FIX 0