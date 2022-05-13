#pragma once

#ifdef CDB_PLATFORM_WINDOWS
	#ifndef RNA_STATICLIB
		#ifdef RNA_DLL
			#define RNAPI __declspec(dllexport)
		#else 
			#define RNAPI __declspec(dllimport)
		#endif
	#else    //Allow for static linking
		#define RNAPI 
	#endif
#else    //declspec isn't needed on linux
	#define RNAPI 
	#define __forceinline inline
#endif

