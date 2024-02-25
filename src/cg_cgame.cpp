#include "cg_cgame.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "dvar.hpp"
#include "in_input.hpp"
#include "pm_pmove.hpp"

static float s_lastMouseX, s_lastMouseY;
static bool  s_firstMouse;

static std::array<cg_t, MAX_LOCAL_CLIENTS> s_cg;


cg_t& CG_GetLocalClientGlobals(int localClientNum) {
	assert(localClientNum < MAX_LOCAL_CLIENTS);
	return s_cg.at(localClientNum);
}

bool CG_LocalClientIsActive(int localClientNum) {
	return CG_GetLocalClientGlobals(localClientNum).active;
}

void CG_ActivateLocalClient(int localClientNum) {
	CG_GetLocalClientGlobals(localClientNum).active = true;
}

void CG_DectivateLocalClient(int localClientNum) {
	CG_GetLocalClientGlobals(localClientNum).active = false;
}

void CG_Init() {
	s_firstMouse = true;
	s_lastMouseX = IN_Mouse_X(0);
	s_lastMouseY = IN_Mouse_Y(0);

	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		cg_t& cg = CG_GetLocalClientGlobals(i);
		cg.camera.pos = glm::vec3(0.0f, 0.0f, 0.0f);
		cg.camera.worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		cg.camera.front = glm::vec3(0.0f, 0.0f, -1.0f);
		cg.camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
		cg.camera.pitch = 0.0f;
		cg.camera.yaw = -90.0f;

		cg.viewport.x = 0.0f;
		cg.viewport.y = 0.0f;
		cg.viewport.w = 1.0f;
		cg.viewport.h = 1.0f;

		cg.fov = Dvar_RegisterLocalFloat(i, "cg_fov", DVAR_FLAG_NONE, 90.0f, 1.0f, 179.0f);
		float w = cg.viewport.w * Dvar_GetInt(*vid_width);
		float h = cg.viewport.h * Dvar_GetInt(*vid_height);
		cg.fovy = FOV_HORZ_TO_VERTICAL(Dvar_GetFloat(*cg.fov), h / w);

		cg.sensitivity = 0.1f;
		cg.active = false;

		PM_GetLocalClientGlobals(i).pm.ps->commandTime = Sys_Milliseconds();
	}

	CG_ActivateLocalClient(0);
}

extern dvar_t* r_fullscreen;
extern dvar_t* r_noBorder;
extern dvar_t* cl_splitscreen;

static void CL_EnterSplitscreen(int activeLocalClient) {
	cg_t& cg0 = CG_GetLocalClientGlobals(0);

	cg0.viewport.x = 0.0f;
	cg0.viewport.y = 0.5f;
	cg0.viewport.w = 1.0f;
	cg0.viewport.h = 0.5f;

	float w0 = cg0.viewport.w * Dvar_GetInt(*vid_width);
	float h0 = cg0.viewport.h * Dvar_GetInt(*vid_height);
	cg0.fovy = FOV_HORZ_TO_VERTICAL(Dvar_GetFloat(*cg0.fov), h0 / w0);

	cg_t& cg1 = CG_GetLocalClientGlobals(1);

	cg1.viewport.x = 0.0f;
	cg1.viewport.y = 0.0f;
	cg1.viewport.w = 1.0f;
	cg1.viewport.h = 0.5f;

	float w1 = cg1.viewport.w * Dvar_GetInt(*vid_width);
	float h1 = cg1.viewport.h * Dvar_GetInt(*vid_height);
	cg1.fovy = FOV_HORZ_TO_VERTICAL(Dvar_GetFloat(*cg1.fov), h1 / w1);

	if (activeLocalClient != 0)
		CG_ActivateLocalClient(0);
	if (activeLocalClient != 1)
		CG_ActivateLocalClient(1);
}

static void CL_LeaveSplitscreen(int activeLocalClient) {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		cg_t& cg = CG_GetLocalClientGlobals(i);
		cg.viewport.x = 0.0f;
		cg.viewport.y = 0.0f;
		cg.viewport.w = 1.0f;
		cg.viewport.h = 1.0f;

		float w = cg.viewport.w * Dvar_GetInt(*vid_width);
		float h = cg.viewport.h * Dvar_GetInt(*vid_height);
		cg.fovy = FOV_HORZ_TO_VERTICAL(Dvar_GetFloat(*cg.fov), h / w);

		if (i != activeLocalClient)
			CG_DectivateLocalClient(i);
	}
}

void CG_Frame(uint64_t) {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		cg_t& cg = CG_GetLocalClientGlobals(i);
		float w = cg.viewport.w * (float)Dvar_GetInt(*vid_width);
		float h = cg.viewport.h * (float)Dvar_GetInt(*vid_height);
		float aspect_inv = h / w;
		cg.fovy = FOV_HORZ_TO_VERTICAL(Dvar_GetFloat(*cg.fov), aspect_inv);
		if(i == 0)Com_DPrintln("{}", cg.fovy);

		if (!CL_HasKbmFocus(i) || CL_KeyFocus(i) != KF_GAME)
			continue;

		if (IN_Key_WasPressedOnCurrentFrame(i, SDLK_m)) {
			CL_GiveKbmFocus(1);
			IN_Key_Clear(0);
			if (!Dvar_GetBool(*cl_splitscreen)) {
				CG_ActivateLocalClient(1);
				CG_DectivateLocalClient(0);
			}
		} else if (IN_Key_WasPressedOnCurrentFrame(i, SDLK_n)) {
			CL_GiveKbmFocus(0);
			IN_Key_Clear(1);
			if (!Dvar_GetBool(*cl_splitscreen)) {
				CG_ActivateLocalClient(0);
				CG_DectivateLocalClient(1);
			}
		}

		if (IN_Key_WasPressedOnCurrentFrame(i, SDLK_j)) {
			Dvar_SetBool(*cl_splitscreen, !Dvar_GetBool(*cl_splitscreen));
		}

		if (Dvar_WasModified(*cl_splitscreen)) {
			if (Dvar_GetBool(*cl_splitscreen)) {
				CL_EnterSplitscreen(i);
			} else {
				CL_LeaveSplitscreen(i);
			}
			Dvar_ClearModified(*cl_splitscreen);
		}


		if (IN_Key_WasPressedOnCurrentFrame(i, SDLK_F10))
			Dvar_SetBool(*r_noBorder, !Dvar_GetBool(*r_noBorder));

		if (IN_Key_WasPressedOnCurrentFrame(i, SDLK_F11))
			Dvar_SetBool(*r_fullscreen, !Dvar_GetBool(*r_fullscreen));

		float vel = 40.0f;
		if (IN_Key_IsDown(i, SDLK_LSHIFT))
			vel *= 1.5f;

		pm_t& pm = PM_GetLocalClientGlobals(i);

		if (IN_Key_IsDown(i, SDLK_w))
			pm.pm.cmd.vel.z =  vel;
		if (IN_Key_IsDown(i, SDLK_s))
			pm.pm.cmd.vel.z = -vel;
		if (IN_Key_IsDown(i, SDLK_a))
			pm.pm.cmd.vel.x = -vel;
		if (IN_Key_IsDown(i, SDLK_d))
			pm.pm.cmd.vel.x = vel;
		if (IN_Key_IsDown(i, SDLK_SPACE))
			pm.pm.cmd.vel.y =  vel;
		if (IN_Key_IsDown(i, SDLK_LCTRL))
			pm.pm.cmd.vel.y = -vel;

		if (pm.pm.cmd.vel.x != 0 && pm.pm.cmd.vel.y != 0) {
			pm.pm.cmd.vel.x *= 0.5f;
			pm.pm.cmd.vel.y *= 0.5f;
		}

		float x = IN_Mouse_X(i);
		float y = IN_Mouse_Y(i);
		if (s_lastMouseX != x || s_lastMouseY != y) {
			if (s_firstMouse) {
				s_lastMouseX = x;
				s_lastMouseY = y;
				s_firstMouse = false;
			}

			float xoff   = x - s_lastMouseX;
			float yoff   = y - s_lastMouseY;
			s_lastMouseX = x;
			s_lastMouseY = y;

			xoff *= cg.sensitivity;
			yoff *= cg.sensitivity;
			pm.pm.cmd.yaw   = -xoff;
			pm.pm.cmd.pitch = -yoff;
		}

		pm.pm.cmd.serverTime = Sys_Milliseconds();
		PmoveSingle(pm.pm, pm.pml);
		cg.camera.pos   = pm.pm.ps->origin;
		cg.camera.front = pm.pml.forward;
	}
}

void CG_Shutdown() {

}