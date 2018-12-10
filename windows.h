#ifndef SE_WINDOWS_H
#define SE_WINDOWS_H

#ifdef _WIN32

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#define USE_NATIVE 0

#include "globals.h"
#include "engine.h"
#include "structs.h"
#include "input.h"

#endif //_WIN32
#endif // !WINDOWS_H
