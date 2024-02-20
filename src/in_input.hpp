#pragma once

#include <span>

#include <SDL3/SDL.h>

#include "com_defs.hpp"

void IN_Init();

void IN_Key_Init();
NO_DISCARD bool  IN_Key_IsDown(int localClientNum, SDL_Keycode k);
NO_DISCARD bool  IN_Key_IsUp(int localClientNum, SDL_Keycode k);
NO_DISCARD bool  IN_Key_IsToggled(int localClientNum, SDL_Keycode k);
NO_DISCARD bool  IN_Key_WasPressedOnCurrentFrame(int localClientNum, SDL_Keycode k);
NO_DISCARD std::span<int> IN_Key_AllPressedOnCurrentFrame(int localClientNum);
           bool  IN_Key_Down(int localClientNum, SDL_Keycode k);
           bool  IN_Key_Up(int localClientNum, SDL_Keycode k);
NO_DISCARD char  IN_Key_Char(int localClientNum, SDL_Keycode k);
void IN_Key_Shutdown();
	  
void IN_Mouse_Init();
NO_DISCARD bool  IN_Mouse_IsDown(int localClientNum, Uint8 button);
NO_DISCARD bool  IN_Mouse_IsUp(int localClientNum, Uint8 button);
           bool  IN_Mouse_Down(int localClientNum, Uint8 button);
           bool  IN_Mouse_Up(int localClientNum, Uint8 button);
           void  IN_Mouse_Move(int localClientNum, float xOff, float yOff);
NO_DISCARD float IN_Mouse_X(int localClientNum);
NO_DISCARD float IN_Mouse_Y(int localClientNum);
void IN_Mouse_Shutdown();

void IN_Key_Clear(int localClientNum);
void IN_Frame();
void IN_Shutdown();