#pragma once

#include <string_view>

enum KeyFocus {
	KF_DEVGUI,
	KF_GAME,
};

void     CL_Init();
void     CL_EnableFpsCounter(int localClientNum, bool enable);
void     CL_Frame();
void	 CL_GiveKbmFocus(int localClientNum);
bool	 CL_HasKbmFocus(int localClientNum);
int      CL_ClientWithKbmFocus();
KeyFocus CL_KeyFocus(int localClientNum);
void     CL_SetKeyFocus(int localClientNum, KeyFocus f);
bool     CL_LoadMap(std::string_view map_name);
void     CL_Shutdown();