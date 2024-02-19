#pragma once

enum KeyFocus {
	KF_DEVGUI,
	KF_GAME,
};

void     CL_Init();
void     CL_EnableFpsCounter(bool enable);
void     CL_DrawFps_f();
void     CL_Frame();
void	 CL_GiveKbmFocus(int localClientNum);
bool	 CL_HasKbmFocus(int localClientNum);
int      CL_ClientWithKbmFocus();
KeyFocus CL_KeyFocus(int localClientNum);
void     CL_SetKeyFocus(int localClientNum, KeyFocus f);
void     CL_Shutdown();