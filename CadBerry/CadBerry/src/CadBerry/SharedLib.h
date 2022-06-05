#pragma once

#ifdef CDB_PLATFORM_LINUX
    #include <dlfcn.h>
#endif
#ifdef CDB_PLATFORM_WINDOWS
    #include "Windows.h"
#endif

namespace CDB
{
    template<typename FuncType>
    inline FuncType GetSharedLibFunc(const std::filesystem::path::value_type* PathToDLL, const char* FuncName)
    {
        #ifdef CDB_PLATFORM_LINUX
            void* LibHandle = dlopen(PathToDLL, RTLD_NOW);

            if (!LibHandle)
            {
                CDB_EditorError("Could not load dll \"{0}\" with error {1}", PathToDLL, dlerror());
                return nullptr;
            }

            //Get the function
            FuncType TargetFunc = (FuncType)dlsym(LibHandle, FuncName);
            if (!TargetFunc)
            {
                CDB_EditorError("Could not locate {0} function in shared library \"{1}\"", FuncName, PathToDLL);
                dlclose(LibHandle);
                return nullptr;
            }
        #endif

        #ifdef CDB_PLATFORM_WINDOWS

            HINSTANCE DLLID = LoadLibrary(PathToDLL);

            if (!DLLID)
            {
                CDB_EditorError("Could not load dll \"{0}\" with error {1}", (const char*)PathToDLL, GetLastError());
                return nullptr;
            }

            //Get the "GetModule" function
            FuncType TargetFunc = (FuncType)GetProcAddress(DLLID, FuncName);
            if (!TargetFunc)
            {
                CDB_EditorError("Could not locate {0} function in dll \"{1}\"", FuncName, (const char*)PathToDLL);
                FreeLibrary(DLLID);
                return nullptr;
            }
        #endif

        return TargetFunc;
    }

    template<typename FuncType>
    inline FuncType GetSharedLibFunc(const char* PathToDLL, const char* FuncName)
    {
#ifdef CDB_PLATFORM_LINUX
        void* LibHandle = dlopen(PathToDLL, RTLD_NOW);

        if (!LibHandle)
        {
            CDB_EditorError("Could not load dll \"{0}\" with error {1}", PathToDLL, dlerror());
            return nullptr;
        }

        //Get the function
        FuncType TargetFunc = (FuncType)dlsym(LibHandle, FuncName);
        if (!TargetFunc)
        {
            CDB_EditorError("Could not locate {0} function in shared library \"{1}\"", FuncName, PathToDLL);
            dlclose(LibHandle);
            return nullptr;
        }
#endif

#ifdef CDB_PLATFORM_WINDOWS
        std::string tmp1 = PathToDLL;
        std::wstring tmp = std::wstring(tmp1.begin(), tmp1.end());    //TODO: fix this mess

        HINSTANCE DLLID = LoadLibrary(tmp.c_str());

        if (!DLLID)
        {
            CDB_EditorError("Could not load dll \"{0}\" with error {1}", PathToDLL, GetLastError());
            return nullptr;
        }

        //Get the "GetModule" function
        FuncType TargetFunc = (FuncType)GetProcAddress(DLLID, FuncName);
        if (!TargetFunc)
        {
            CDB_EditorError("Could not locate {0} function in dll \"{1}\"", FuncName, PathToDLL);
            FreeLibrary(DLLID);
            return nullptr;
        }
#endif

        return TargetFunc;
    }
}