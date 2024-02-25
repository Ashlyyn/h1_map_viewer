#include "cl_client.hpp"

#include <cassert>

#include "cg_cgame.hpp"
#include "db_files.hpp"
#include "dvar.hpp"
#include "gfx.hpp"
#include "gfx_text.hpp"
#include "in_input.hpp"

struct cll_t {
	KeyFocus keyfocus;
	bool drawDevGui;
	size_t fpsTextDrawId;
};

std::array<cll_t, MAX_LOCAL_CLIENTS> s_cll;

struct cl_t {
	dvar_t* drawfps;
	bool hasKbmFocus;
};

std::array<cl_t, MAX_LOCAL_CLIENTS> s_cl;

dvar_t* cl_splitscreen;

static uint64_t s_lastFpsDrawTime;
static uint64_t s_lastFpsDrawDelta;

cll_t& CL_GetLocalClientLocals(int localClientNum) {
	return s_cll.at(localClientNum);
}

cl_t& CL_GetLocalClientGlobals(int localClientNum) {
	return s_cl.at(localClientNum);
}

void CL_Init() {
	cl_splitscreen = &Dvar_RegisterBool("cl_splitscreen", DVAR_FLAG_NONE, false);

	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		cl_t& cl = CL_GetLocalClientGlobals(i);
		cl.drawfps = Dvar_RegisterLocalBool(i, "cl_drawfps", DVAR_FLAG_NONE, false);
		CL_GetLocalClientLocals(i).drawDevGui = false;
		CL_SetKeyFocus(i, KF_GAME);
		assert(R_AddTextDraw(
			i, NULL,
			Com_Format("FPS: {:.0f}", 1000.0f / s_lastFpsDrawDelta),
			0.9502f, 0.9502f, 0.5f, 0.5f,
			glm::vec3(0.5, 0.8f, 0.2f), Dvar_GetBool(*cl.drawfps), true,
			CL_GetLocalClientLocals(i).fpsTextDrawId
		));
	}

	CL_GiveKbmFocus(0);
	bool b = CL_LoadMap("c40.map");
	assert(b);
}

void CL_EnableFpsCounter(int localClientNum, bool enable) {
	Dvar_SetBool(*CL_GetLocalClientGlobals(localClientNum).drawfps, enable);
}

void CL_DrawFps(int localClientNum) {
	R_UpdateTextDraw(
		localClientNum, CL_GetLocalClientLocals(localClientNum).fpsTextDrawId,
		Com_Format("FPS: {:.0f}", 1000.0f / (float)s_lastFpsDrawDelta)
	);
}

void CL_Frame() {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		cl_t& cl = CL_GetLocalClientGlobals(i);
		cll_t& cll = CL_GetLocalClientLocals(i);
		R_ActivateTextDraw(i, cll.fpsTextDrawId, Dvar_GetBool(*cl.drawfps));

		if (Sys_Milliseconds() - s_lastFpsDrawTime > 40) {
			s_lastFpsDrawTime = Sys_Milliseconds();
			s_lastFpsDrawDelta = Com_LastFrameTimeDelta();
		}

		if (Dvar_GetBool(*cl.drawfps))
			CL_DrawFps(i);

		cll.drawDevGui = IN_Key_IsToggled(i, SDLK_BACKQUOTE);
		if (cll.drawDevGui)
			CL_SetKeyFocus(i, KF_DEVGUI);
		else
			CL_SetKeyFocus(i, KF_GAME);
	}	
} 

void CL_GiveKbmFocus(int localClientNum) {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++)
		CL_GetLocalClientGlobals(i).hasKbmFocus = i == localClientNum;
}

bool CL_HasKbmFocus(int localClientNum) {
	return CL_GetLocalClientGlobals(localClientNum).hasKbmFocus;
}

int CL_ClientWithKbmFocus() {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		if (CL_HasKbmFocus(i))
			return i;
	}

	assert(false);
	return -1;
}

KeyFocus CL_KeyFocus(int localClientNum) {
	return CG_GetLocalClientGlobals(localClientNum).keyfocus;
}

void CL_SetKeyFocus(int localClientNum, KeyFocus f) {
	CG_GetLocalClientGlobals(localClientNum).keyfocus = f;
}

bool CL_LoadMap(std::string_view map_name) {
	return DB_LoadMap(map_name).pos >= 0;
}

void CL_Shutdown() {
	Dvar_SetBool(*cl_splitscreen, false);
	Dvar_Unregister("cl_splitscreen");
	cl_splitscreen = nullptr;

	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		cl_t& cl = CL_GetLocalClientGlobals(i);
		Dvar_SetBool(*cl.drawfps, false);
		Dvar_UnregisterLocal(i, "cl_drawfps");
		cl.drawfps = nullptr;

		cll_t& cll = CL_GetLocalClientLocals(i);
		cll.drawDevGui = false;
		cll.keyfocus = KF_GAME;
	}
}