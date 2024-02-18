#include <cassert>

#include "cl.hpp"
#include "gfx.hpp"
#include "gfx_text.hpp"
#include "com_defs.hpp"
#include "com_print.hpp"
#include "input.hpp"
#include "cmd_commands.hpp"
#include "dvar.hpp"

dvar_t* cl_drawfps;

KeyFocus cl_keyfocus;
bool cl_drawDevGui;

static size_t cl_fpsTextDrawId;
static uint64_t s_lastFpsDrawTime;
static uint64_t s_lastFpsDrawDelta;

void CL_Init() {
	cl_drawfps = &Dvar_RegisterBool("cl_drawfps", (dvarFlags_t)0, false);
	assert(R_AddTextDraw(
		NULL, Com_Format("FPS: {:.0f}", 1000.0f / s_lastFpsDrawDelta),
		//0.79125, 0.9502, 0.3, 0.3,
		0.9502f, 0.9502f, 0.5f, 0.5f,
		glm::vec3(0.5, 0.8f, 0.2f), Dvar_GetBool(*cl_drawfps), true, cl_fpsTextDrawId
	));

	cl_drawDevGui = false;
	CL_SetKeyFocus(KF_GAME);
}

void CL_EnableFpsCounter(bool enable) {
	Dvar_SetBool(*cl_drawfps, enable);
}

void CL_DrawFps() {
	R_UpdateTextDraw(cl_fpsTextDrawId, Com_Format("FPS: {:.0f}", 1000.0f / (float)s_lastFpsDrawDelta));
}

void CL_Frame() {
	R_ActivateTextDraw(cl_fpsTextDrawId, Dvar_GetBool(*cl_drawfps));

	if (Sys_Milliseconds() - s_lastFpsDrawTime > 40) {
		s_lastFpsDrawTime  = Sys_Milliseconds();
		s_lastFpsDrawDelta = Com_LastFrameTimeDelta();
	}

	if (Dvar_GetBool(*cl_drawfps))
		CL_DrawFps();

	cl_drawDevGui = IN_Key_IsToggled(SDLK_BACKQUOTE);
	if (cl_drawDevGui) 
		CL_SetKeyFocus(KF_DEVGUI);
	else 
		CL_SetKeyFocus(KF_GAME);
} 

KeyFocus CL_KeyFocus() {
	return cl_keyfocus;
}

void CL_SetKeyFocus(KeyFocus f) {
	cl_keyfocus = f;
}

void CL_Shutdown() {
	Dvar_SetBool(*cl_drawfps, false);
	cl_drawDevGui = false;
	cl_keyfocus = KF_GAME;
}