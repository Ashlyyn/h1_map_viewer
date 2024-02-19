#pragma once

#include <string>

void DevGui_Init();
bool DevGui_HasText(int localClientNum);
std::string DevGui_TakeText(int localClientNum);
void DevGui_Frame();
void DevGui_Shutdown();