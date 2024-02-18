#pragma once

enum KeyFocus {
	KF_DEVGUI,
	KF_GAME,
};

void     CL_Init();
void     CL_EnableFpsCounter(bool enable);
void     CL_DrawFps_f();
void     CL_Frame();
KeyFocus CL_KeyFocus();
void     CL_SetKeyFocus(KeyFocus f);
void     CL_Shutdown();