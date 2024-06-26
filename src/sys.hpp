#pragma once

#include "com_defs.hpp"

enum thread_t {
	THREAD_MAIN = 0,
};

constexpr inline int VID_WIDTH_DEFAULT  = 1024;
constexpr inline int VID_HEIGHT_DEFAULT = 768;

constexpr inline size_t SYS_MAX_ARGV = 256;

void Sys_Init(const char** argv);
bool Sys_HandleEvent();
bool Sys_AwaitingThread(thread_t thread);
void Sys_Shutdown();
//A_NO_RETURN Sys_NormalExit(int ec);
//uint64_t Sys_Milliseconds();
