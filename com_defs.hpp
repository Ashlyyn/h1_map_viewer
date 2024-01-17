#pragma once

#include <cstdint>

#if defined _WIN32 || defined _WIN64
#define TARGET_OS_IS_WINDOWS 1
#endif // _WIND32

#define NO_RETURN  [[noreturn]]  void
#define NO_DISCARD [[nodiscard]]

#if TARGET_OS_IS_WINDOWS
#define IN _In_
#else // TARGET_OS_IS_WINDOWS
#define IN
#endif // TARGET_OS_IS_WINDOWS

#if TARGET_OS_IS_WINDOWS
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

// can't #include sys.hpp because sys.hpp #includes this file
NO_RETURN Sys_NormalExit(int ec);
uint64_t Sys_Milliseconds();

#define NOP() ;

bool Com_Init();
bool Com_Frame();
void Com_Shutdown();