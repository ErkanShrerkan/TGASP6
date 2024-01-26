// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"
#include <ThirdParty\ImGui\imgui.h>
//#include <rapidjson/document.h>

// Frequently used stuff
#include <CommonUtilities/MathBundle.hpp>
#include "CommonUtilities.h"

template <int bytes, class type = unsigned char>
struct UnusedSpace
{
	__int8 myBytes[bytes * sizeof(type)];
};

#define C_ARRAY_SIZE(aCArray) (sizeof(aCArray) / sizeof(*(aCArray)))

#include "DebugConsole.h"
#define pout Singleton<SE::Debug::CDebugConsole>()
#define pend SE::Debug::CDebugConsole::EConsoleStream::EndStream
#define pwarn pout
#define perr pout
// ^^ perror is taken in <tchar.h>

// Indicates that this pointer is not owned by this object
template <class T>
using NotMy = T*;

typedef uint32_t hash_t;

// PIX


#ifndef _RETAIL

#else

// Define used PIX stuff here such as
// PIXBeginEvent PIXEndEvent PIXScopedEvent

#endif

// End of pch.h
#endif
