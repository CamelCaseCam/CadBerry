#pragma once
#include "CadBerry.h"

extern bool Aborted;
#define FatalError(...) CDB_BuildError(__VA_ARGS__);\
CDB_BuildError("Encountered fatal error, aborting compilation. Any errors after this message are a result of that process and not user error");\
CDB_BuildTrace("\n______________________________________________________________________________________");\
Aborted = true

#define CheckFatal do { if (Aborted) return; } while(0)