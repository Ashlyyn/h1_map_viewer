#pragma once

#include "com_defs.hpp"

enum thread_t {
	THREAD_MAIN = 0,
	THREAD_DEVCON,
};

constexpr inline int VID_WIDTH_DEFAULT  = 1024;
constexpr inline int VID_HEIGHT_DEFAULT = 768;

void Sys_InitThreads();
void Sys_Init();
bool Sys_HandleEvent();
void Sys_Shutdown();
bool Sys_SpawnDevConThread(int(*DevConThread)(void*));
//NO_RETURN Sys_NormalExit(int ec);
//uint64_t Sys_Milliseconds();