#pragma once

#include <Engine\Timer.h>
#define DELTA_TIME Singleton<CommonUtilities::Timer>().GetDeltaTime()

#define ENGINE (SE::CEngine::GetInstance())
#define CAMERA (ENGINE->GetActiveScene()->GetMainCamera())

// Get meta variables
#ifdef __has_include
#   if __has_include("../Meta/USER_VARIABLE.h")
#       include "../Meta/USER_VARIABLE.h"
#   endif
#endif // __has_include

#pragma region MUV_USER
#ifdef MUV_USER
#if MUV_USER == MUV_ERIC
#define printe(...) printf(__VA_ARGS__)
#else
#define printe(...)
#endif
#if MUV_USER == MUV_JESPER
#define printje(...) printf(__VA_ARGS__)
#else
#define printje(...)
#endif
#if MUV_USER == MUV_JIMMI
#define printji(...) printf(__VA_ARGS__)
#else
#define printji(...)
#endif
#if MUV_USER == MUV_JOEL
#define printjo(...) printf(__VA_ARGS__)
#else
#define printjo(...)
#endif
#if MUV_USER == MUV_SIMON
#define printsi(...) printf(__VA_ARGS__)
#else
#define printsi(...)
#endif
#else
#define printe(XELZPX)
#define printje(XELZPX)
#define printji(XELZPX)
#define printjo(XELZPX)
#define printsi(XELZPX)
#endif // MUV_USER
#pragma endregion
