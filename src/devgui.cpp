#include "cl.hpp"
#include "gfx.hpp"
#include "gfx_text.hpp"
#include "input.hpp"

size_t devgui_promptDrawId;
std::string devgui_buffer;

void DevGui_Init() {
	devgui_buffer = "> ";

	assert(R_AddTextDraw(
		nullptr, devgui_buffer,
		0.1f, 0.9f, 1.0f, 1.0f, glm::vec3(0.9f, 0.2f, 0.2f),
		false, false, devgui_promptDrawId)
	);
}

bool DevGui_HasText() {
	return devgui_buffer.contains('\n');
}

std::string DevGui_TakeText() {
	size_t pos = devgui_buffer.find('\n');
	size_t len = pos - 2;
	std::string input = devgui_buffer.substr(2, len);
	devgui_buffer.erase(2, len + 1);
	return input;
}

void DevGui_Frame() {
	if (CL_KeyFocus() != KF_DEVGUI) {
		R_ActivateTextDraw(devgui_promptDrawId, false);
		return;
	}

	for (auto k : IN_Key_AllPressedOnCurrentFrame()) {
		char c = IN_Key_Char(k);
		if (c != 0 && k != SDLK_BACKQUOTE)
			devgui_buffer.push_back(c);
		else if (k == SDLK_BACKSPACE && devgui_buffer.length() > 2)
			devgui_buffer.pop_back();
		else if (k == SDLK_DELETE)
			devgui_buffer = "> ";
	}

	R_UpdateTextDraw(devgui_promptDrawId, devgui_buffer);
	R_ActivateTextDraw(devgui_promptDrawId, true);
}

void DevGui_Shutdown() {
	R_RemoveTextDraw(devgui_promptDrawId);

	devgui_buffer.clear();
}