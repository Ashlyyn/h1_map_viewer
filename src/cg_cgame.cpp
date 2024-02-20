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

		cg.fov = 120.0f;
		cg.sensitivity = 0.1f;
		cg.active = false;

		PM_GetLocalClientGlobals(i).pm.ps->commandTime = Sys_Milliseconds();
	}

	CG_ActivateLocalClient(0);
}

/*
void CG_MoveForward(int localClientNum, float vel) {
	cg_t& cg = CG_GetLocalClientGlobals(localClientNum);
	cg.camera.pos += vel * cg.camera.front;
}

void CG_MoveBackward(int localClientNum, float vel) {
	cg_t& cg = CG_GetLocalClientGlobals(localClientNum);
	cg.camera.pos -= vel * cg.camera.front;
}

void CG_StrafeLeft(int localClientNum, float vel) {
	cg_t& cg = CG_GetLocalClientGlobals(localClientNum);
	cg.camera.pos -= glm::normalize(
		glm::cross(cg.camera.front, cg.camera.up)
	) * vel;
}

void CG_StrafeRight(int localClientNum, float vel) {
	cg_t& cg = CG_GetLocalClientGlobals(localClientNum);
	cg.camera.pos += glm::normalize(
		glm::cross(cg.camera.front, cg.camera.up)
	) * vel;
}

void CG_Ascend(int localClientNum, float vel) {
	cg_t& cg = CG_GetLocalClientGlobals(localClientNum);
	cg.camera.pos += vel * cg.camera.up;
}

void CG_Descend(int localClientNum, float vel) {
	cg_t& cg = CG_GetLocalClientGlobals(localClientNum);
	cg.camera.pos -= vel * cg.camera.up;
}
*/

void CG_Look(int localClientNum, float x, float y) {
	if (s_firstMouse) {
		s_lastMouseX = x;
		s_lastMouseY = y;
		s_firstMouse = false;
	}

	float xoff = x - s_lastMouseX;
	float yoff = y - s_lastMouseY;
	s_lastMouseX = x;
	s_lastMouseY = y;

	cg_t& cg = CG_GetLocalClientGlobals(localClientNum);
	xoff *= cg.sensitivity;
	yoff *= cg.sensitivity;

	cg.camera.yaw   -= xoff;
	cg.camera.pitch += yoff;

	if (cg.camera.pitch > 89.0f)
		cg.camera.pitch = 89.0f;
	if (cg.camera.pitch < -89.0f)
		cg.camera.pitch = -89.0f;

	glm::vec3 front;
	front.x = 
		cos(glm::radians(cg.camera.yaw)) * cos(glm::radians(cg.camera.pitch));
	front.y = 
		sin(glm::radians(cg.camera.pitch));
	front.z = 
		sin(glm::radians(cg.camera.yaw)) * cos(glm::radians(cg.camera.pitch));
	cg.camera.front = glm::normalize(front);
}

extern dvar_t* r_fullscreen;
extern dvar_t* r_noBorder;

void CG_Frame(uint64_t deltaTime) {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		if (!CL_HasKbmFocus(i) || CL_KeyFocus(i) != KF_GAME)
			continue;

		if (IN_Key_WasPressedOnCurrentFrame(i, SDLK_m)) {
			CG_ActivateLocalClient(1);
			CL_GiveKbmFocus(1);
			IN_Key_Clear(0);
			CG_DectivateLocalClient(0);
		} else if (IN_Key_WasPressedOnCurrentFrame(i, SDLK_n)) {
			CG_ActivateLocalClient(0);
			CL_GiveKbmFocus(0);
			IN_Key_Clear(1);
			CG_DectivateLocalClient(1);
		}

		if (IN_Key_WasPressedOnCurrentFrame(i, SDLK_F10))
			Dvar_SetBool(*r_noBorder, !Dvar_GetBool(*r_noBorder));

		if (IN_Key_WasPressedOnCurrentFrame(i, SDLK_F11))
			Dvar_SetBool(*r_fullscreen, !Dvar_GetBool(*r_fullscreen));

		float vel = 2.5f * ((float)deltaTime);
		if (IN_Key_IsDown(i, SDLK_LSHIFT))
			vel *= 2;

		pm_t& pm = PM_GetLocalClientGlobals(i);

		if (IN_Key_IsDown(i, SDLK_w))
			pm.pm.cmd.vel.z =  vel;
			//CG_MoveForward(i, vel);
		if (IN_Key_IsDown(i, SDLK_s))
			pm.pm.cmd.vel.z = -vel;
			//CG_MoveBackward(i, vel);
		if (IN_Key_IsDown(i, SDLK_a))
			pm.pm.cmd.vel.x = -vel;
			//CG_StrafeLeft(i, vel);
		if (IN_Key_IsDown(i, SDLK_d))
			pm.pm.cmd.vel.x =  vel;
			//CG_StrafeRight(i, vel);
		if (IN_Key_IsDown(i, SDLK_SPACE))
			pm.pm.cmd.vel.y =  vel;
			//CG_Ascend(i, vel);
		if (IN_Key_IsDown(i, SDLK_LCTRL))
			pm.pm.cmd.vel.y = -vel;
			//CG_Descend(i, vel);

		//Com_DPrintln("x={}, y={}", IN_Mouse_X(), IN_Mouse_Y());
		cg_t& cg = CG_GetLocalClientGlobals(i);
		float x = IN_Mouse_X(i);
		float y = IN_Mouse_Y(i);
		if (s_lastMouseX != x || s_lastMouseY != y) {
			if (s_firstMouse) {
				s_lastMouseX = x;
				s_lastMouseY = y;
				s_firstMouse = false;
			}

			float xoff = x - s_lastMouseX;
			float yoff = y - s_lastMouseY;
			s_lastMouseX = x;
			s_lastMouseY = y;

			xoff *= cg.sensitivity;
			yoff *= cg.sensitivity;
			pm.pm.cmd.yaw   = xoff;
			pm.pm.cmd.pitch = yoff;
			//CG_Look(i, IN_Mouse_X(i), IN_Mouse_Y(i));
		}

		pm.pm.cmd.serverTime = Sys_Milliseconds();
		PmoveSingle(pm.pm, pm.pml);
		cg.camera.pos = pm.pm.ps->origin;
		cg.camera.front = pm.pml.forward;
	}
}

void CG_Shutdown() {

}