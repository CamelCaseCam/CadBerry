#pragma once
#include <string>

#ifdef CDB_PLATFORM_WINDOWS
#include <windows.h>
#endif

#ifdef CDB_PLATFORM_LINUX
#include <cstdlib>
#endif

namespace CDB
{
    inline void ExecCommand(std::string Program, std::string Args)
    {
        #ifdef CDB_PLATFORM_WINDOWS
        std::wstring tmp1 = std::wstring(Args.begin(), Args.end());

        LPCWSTR params = tmp1.c_str();

        std::wstring tmp3 = std::wstring(Program.begin(), Program.end());
        LPCWSTR ExeName = tmp3.c_str();
        ShellExecute(nullptr, NULL, ExeName, params, 0, SW_SHOW);
        #endif

        #ifdef CDB_PLATFORM_LINUX
        system((Program + " " + Args + " &").c_str());
        #endif
    }
}