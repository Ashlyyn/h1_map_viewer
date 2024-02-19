#pragma once

#include <cstdint>

#ifdef _WIN32
#define TARGET_OS_IS_WINDOWS 1
#endif // _WIND32

#define NO_RETURN  [[noreturn]]  void
#define NO_DISCARD [[nodiscard]]

#if TARGET_OS_IS_WINDOWS
#ifdef IN
#undef IN
#endif
#define IN _In_
#else // TARGET_OS_IS_WINDOWS
#define IN
#endif // TARGET_OS_IS_WINDOWS

#if TARGET_OS_IS_WINDOWS
#ifdef OUT
#undef OUT
#endif
#define OUT _Out_
#else // TARGET_OS_IS_WINDOWS
#define OUT
#endif // TARGET_OS_IS_WINDOWS

#if TARGET_OS_IS_WINDOWS
#define INOUT _Inout_
#else // TARGET_OS_IS_WINDOWS
#define INOUT
#endif // TARGET_OS_IS_WINDOWS

#if TARGET_OS_IS_WINDOWS
#define OPTIONAL_OUT _Out_opt_
#else // TARGET_OS_IS_WINDOWS
#define OPTIONAL_OUT
#endif // TARGET_OS_IS_WINDOWS

#if TARGET_OS_IS_WINDOWS
#define OPTIONAL_IN _In_opt_
#else // TARGET_OS_IS_WINDOWS
#define OPTIONAL_IN
#endif // TARGET_OS_IS_WINDOWS

// can't #include sys.hpp because sys.hpp #includes this file
NO_RETURN Sys_NormalExit(int ec);
uint64_t Sys_Milliseconds();

constexpr size_t MAX_LOCAL_CLIENTS = 4;

struct RectDef {
	float x, y, w, h;
};

#define NOP() ;

bool     Com_Init();
bool     Com_Frame();
uint64_t Com_LastFrameTime();
uint64_t Com_LastFrameTimeDelta();
void     Com_Shutdown();