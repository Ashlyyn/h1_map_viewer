#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//#include "com_print.hpp"
#include "cg_cgame.hpp"
#include "input.hpp"

struct CGameClient {
	GfxCamera camera;
} cg;

float cg_fov;
float cg_baseVelocity;
float cg_sensitivity;

static float s_lastMouseX, s_lastMouseY;
static bool  s_firstMouse;

void CG_Init() {
	cg.camera.pos     = glm::vec3( 0.0f,  0.0f,  0.0f );
	cg.camera.worldUp = glm::vec3( 0.0f,  1.0f,  0.0f );
	cg.camera.front   = glm::vec3( 0.0f,  0.0f, -1.0f );
	cg.camera.up      = glm::vec3( 0.0f,  1.0f,  0.0f );
	cg.camera.pitch   =  0.0f;
	cg.camera.yaw     = -90.0f;

	s_firstMouse = true;
	s_lastMouseX = IN_Mouse_X();
	s_lastMouseY = IN_Mouse_Y();

	cg_fov = 120.0f;
	cg_baseVelocity = 2.5f;
	cg_sensitivity  = 0.1f;
}

void CG_MoveForward(float vel) {
	cg.camera.pos += vel * cg.camera.front;
}

void CG_MoveBackward(float vel) {
	cg.camera.pos -= vel * cg.camera.front;
}

void CG_StrafeLeft(float vel) {
	cg.camera.pos -= glm::normalize(
		glm::cross(cg.camera.front, cg.camera.up)
	) * vel;
}

void CG_StrafeRight(float vel) {
	cg.camera.pos += glm::normalize(
		glm::cross(cg.camera.front, cg.camera.up)
	) * vel;
}

void CG_Ascend(float vel) {
	cg.camera.pos += vel * cg.camera.up;
}

void CG_Descend(float vel) {
	cg.camera.pos -= vel * cg.camera.up;
}

void CG_Look(float x, float y) {
	if (s_firstMouse) {
		s_lastMouseX = x;
		s_lastMouseY = y;
		s_firstMouse = false;
	}

	float xoff = x - s_lastMouseX;
	float yoff = y - s_lastMouseY;
	s_lastMouseX = x;
	s_lastMouseY = y;

	xoff *= cg_sensitivity;
	yoff *= cg_sensitivity;

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
	float vel = 2.5f * ((float)deltaTime / 1000.0f);
	if (IN_Key_IsDown(SDLK_LSHIFT))
		vel *= 2;

	if (IN_Key_IsDown(SDLK_w))
		CG_MoveForward(vel);
	if (IN_Key_IsDown(SDLK_s))
		CG_MoveBackward(vel);
	if (IN_Key_IsDown(SDLK_a))
		CG_StrafeLeft(vel);
	if (IN_Key_IsDown(SDLK_d))
		CG_StrafeRight(vel);
	if (IN_Key_IsDown(SDLK_SPACE))
		CG_Ascend(vel);
	if (IN_Key_IsDown(SDLK_LCTRL))
		CG_Descend(vel);

	//Com_DPrintln("x={}, y={}", IN_Mouse_X(), IN_Mouse_Y());
	if (s_lastMouseX != IN_Mouse_X() || s_lastMouseY != IN_Mouse_Y()) {
		CG_Look(IN_Mouse_X(), IN_Mouse_Y());
	}
	
}

GfxCamera& CG_Camera() {
	return cg.camera;
}

float CG_Fov() {
	return cg_fov;
}

void CG_Shutdown() {

}