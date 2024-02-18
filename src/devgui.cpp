#include "cl.hpp"
#include "gfx.hpp"
#include "gfx_text.hpp"

size_t s_focusDrawId;

void DevGui_Init() {
	assert(R_AddTextDraw(
		nullptr, "DevGui has focus", 
		0.1f, 0.9f, 1.0f, 1.0f, glm::vec3(0.9f, 0.2f, 0.2f), 
		false, false, s_focusDrawId)
	);
}

bool DevGui_HasText() {
	return false;
}

void DevGui_Frame() {
	if (CL_KeyFocus() != KF_DEVGUI) {
		R_ActivateTextDraw(s_focusDrawId, false);
		return;
	}

	R_ActivateTextDraw(s_focusDrawId, true);
}

void DevGui_Shutdown() {
	R_RemoveTextDraw(s_focusDrawId);
}