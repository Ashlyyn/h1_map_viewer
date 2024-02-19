#include <cassert>

#include "cl.hpp"
#include "cg_cgame.hpp"
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

static std::array<size_t, MAX_LOCAL_CLIENTS> cl_fpsTextDrawId;
static uint64_t s_lastFpsDrawTime;
static uint64_t s_lastFpsDrawDelta;

static void CL_ClientInit(int localClientNum);

void CL_Init() {
	cl_drawfps = &Dvar_RegisterBool("cl_drawfps", (dvarFlags_t)0, false);
	cl_drawDevGui = false;

	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++)
		CL_ClientInit(i);
}

void CL_ClientInit(int localClientNum) {
	CL_SetKeyFocus(localClientNum, KF_GAME);
	assert(R_AddTextDraw(
		localClientNum, NULL, 
		Com_Format("FPS: {:.0f}", 1000.0f / s_lastFpsDrawDelta),
		0.9502f, 0.9502f, 0.5f, 0.5f,
		glm::vec3(0.5, 0.8f, 0.2f), Dvar_GetBool(*cl_drawfps), true,
		cl_fpsTextDrawId.at(localClientNum)
	));
}

void CL_EnableFpsCounter(bool enable) {
	Dvar_SetBool(*cl_drawfps, enable);
}

void CL_DrawFps(int localClientNum) {
	R_UpdateTextDraw(
		localClientNum, cl_fpsTextDrawId.at(localClientNum),
		Com_Format("FPS: {:.0f}", 1000.0f / (float)s_lastFpsDrawDelta)
	);
}

void CL_Frame() {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		R_ActivateTextDraw(0, cl_fpsTextDrawId.at(i), Dvar_GetBool(*cl_drawfps));

		if (Sys_Milliseconds() - s_lastFpsDrawTime > 40) {
			s_lastFpsDrawTime = Sys_Milliseconds();
			s_lastFpsDrawDelta = Com_LastFrameTimeDelta();
		}

		if (Dvar_GetBool(*cl_drawfps))
			CL_DrawFps(i);

		cl_drawDevGui = IN_Key_IsToggled(SDLK_BACKQUOTE);
		if (cl_drawDevGui)
			CL_SetKeyFocus(i, KF_DEVGUI);
		else
			CL_SetKeyFocus(i, KF_GAME);
	}	
} 

KeyFocus CL_KeyFocus(int localClientNum) {
	return CG_GetLocalClientGlobals(localClientNum).keyfocus;
}

void CL_SetKeyFocus(int localClientNum, KeyFocus f) {
	CG_GetLocalClientGlobals(localClientNum).keyfocus = f;
}

void CL_Shutdown() {
	Dvar_SetBool(*cl_drawfps, false);
	cl_drawDevGui = false;
	cl_keyfocus = KF_GAME;
}