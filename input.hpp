#include <SDL3/SDL.h>
 
void  IN_Init();

void  IN_Key_Init();
bool  IN_Key_IsDown(SDL_Keycode k);
bool  IN_Key_IsUp(SDL_Keycode k);
bool  IN_Key_Down(SDL_Keycode k);
bool  IN_Key_Up(SDL_Keycode k);
void  IN_Key_Shutdown();
	  
void  IN_Mouse_Init();
bool  IN_Mouse_IsDown(Uint8 button);
bool  IN_Mouse_IsUp(Uint8 button);
bool  IN_Mouse_Down(Uint8 button);
bool  IN_Mouse_Up(Uint8 button);
void  IN_Mouse_Move(float xOff, float yOff);
float IN_Mouse_X();
float IN_Mouse_Y();
void  IN_Mouse_Shutdown();
void  IN_Mouse_Shutdown();

void  IN_Shutdown();