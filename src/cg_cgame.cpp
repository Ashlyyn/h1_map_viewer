#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "cg_cgame.hpp"
#include "input.hpp"
#include "cl.hpp"

static float s_lastMouseX, s_lastMouseY;
static bool  s_firstMouse;

static std::array<cg_t, MAX_LOCAL_CLIENTS> s_cg;

void CG_Init() {
	s_firstMouse = true;
	s_lastMouseX = IN_Mouse_X();
	s_lastMouseY = IN_Mouse_Y();

	for (auto& cg : s_cg) {
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
	}

	s_cg[0].hasKbmFocus = true;
	s_cg[0].active = true;
}

cg_t& CG_GetLocalClientGlobals(int localClientNum) {
	assert(localClientNum < MAX_LOCAL_CLIENTS);
	return s_cg.at(localClientNum);
}

void CG_ActivateLocalClient(int localClientNum) {
	CG_GetLocalClientGlobals(localClientNum).active = true;
}

void CG_DectivateLocalClient(int localClientNum) {
	CG_GetLocalClientGlobals(localClientNum).active = false;
}

void CG_GiveKbmFocus(int localClientNum) {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++)
		CG_GetLocalClientGlobals(i).hasKbmFocus = i == localClientNum; 
}

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

void CG_Frame(uint64_t deltaTime) {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		if (CL_KeyFocus(i) != KF_GAME)
			return;

		if (IN_Key_WasPressedOnCurrentFrame(SDLK_n)) {
			CG_ActivateLocalClient(1);
			CG_GiveKbmFocus(1);
			CG_DectivateLocalClient(0);
		}
		else if (IN_Key_WasPressedOnCurrentFrame(SDLK_m)) {
			CG_DectivateLocalClient(1);
			CG_GiveKbmFocus(0);
			CG_ActivateLocalClient(0);
		}

		float vel = 2.5f * ((float)deltaTime / 1000.0f);
		if (IN_Key_IsDown(SDLK_LSHIFT))
			vel *= 2;

		if (!CG_GetLocalClientGlobals(i).hasKbmFocus)
			continue;

		if (IN_Key_IsDown(SDLK_w))
			CG_MoveForward(i, vel);
		if (IN_Key_IsDown(SDLK_s))
			CG_MoveBackward(i, vel);
		if (IN_Key_IsDown(SDLK_a))
			CG_StrafeLeft(i, vel);
		if (IN_Key_IsDown(SDLK_d))
			CG_StrafeRight(i, vel);
		if (IN_Key_IsDown(SDLK_SPACE))
			CG_Ascend(i, vel);
		if (IN_Key_IsDown(SDLK_LCTRL))
			CG_Descend(i, vel);

		//Com_DPrintln("x={}, y={}", IN_Mouse_X(), IN_Mouse_Y());
		if (s_lastMouseX != IN_Mouse_X() || s_lastMouseY != IN_Mouse_Y()) {
			CG_Look(i, IN_Mouse_X(), IN_Mouse_Y());
		}
		break;
	}
}

void CG_Shutdown() {

}