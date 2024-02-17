#pragma once

#include "com_defs.hpp"

void Sys_Init();
bool Sys_HandleEvent();
void Sys_Shutdown();
//NO_RETURN Sys_NormalExit(int ec);
//uint64_t Sys_Milliseconds();