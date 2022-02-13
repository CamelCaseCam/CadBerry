#pragma once

#ifndef RNA_STATICLIB
	#ifdef RNA_DLL
		#define RNAPI __declspec(dllexport)
	#else 
		#define RNAPI __declspec(dllimport)
	#endif
#else    //Allow for static linking
	#define RNAPI 
#endif