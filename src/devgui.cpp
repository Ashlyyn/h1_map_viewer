#include <array>

#include "cl.hpp"
#include "gfx.hpp"
#include "gfx_text.hpp"
#include "input.hpp"

std::array<size_t, MAX_LOCAL_CLIENTS> devgui_promptDrawId;
std::array<std::string, MAX_LOCAL_CLIENTS> devgui_buffer;

void DevGui_Init() {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		devgui_buffer.at(i) = "> ";

		assert(R_AddTextDraw(
			i, nullptr, devgui_buffer.at(i),
			0.1f, 0.9f, 1.0f, 1.0f, glm::vec3(0.9f, 0.2f, 0.2f),
			false, false, devgui_promptDrawId.at(i)
		));
	}
}

bool DevGui_HasText(int localClientNum) {
	return devgui_buffer.at(localClientNum).contains('\n');
}

std::string DevGui_TakeText(int localClientNum) {
	size_t pos = devgui_buffer.at(localClientNum).find('\n');
	size_t len = pos - 2;
	std::string input = devgui_buffer.at(localClientNum).substr(2, len);
	devgui_buffer.at(localClientNum).erase(2, len + 1);
	return input;
}

void DevGui_Frame() {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		if (CL_KeyFocus(i) != KF_DEVGUI) {
			R_ActivateTextDraw(i, devgui_promptDrawId.at(i), false);
			return;
		}

		for (auto k : IN_Key_AllPressedOnCurrentFrame()) {
			char c = IN_Key_Char(k);
			if (c != 0 && k != SDLK_BACKQUOTE)
				devgui_buffer.at(i).push_back(c);
			else if (k == SDLK_BACKSPACE && devgui_buffer.at(i).length() > 2)
				devgui_buffer.at(i).pop_back();
			else if (k == SDLK_DELETE)
				devgui_buffer.at(i) = "> ";
		}

		R_UpdateTextDraw(i, devgui_promptDrawId.at(i), devgui_buffer.at(i));
		R_ActivateTextDraw(i, devgui_promptDrawId.at(i), true);
	}
}

void DevGui_Shutdown() {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		R_RemoveTextDraw(i, devgui_promptDrawId.at(i));
		devgui_buffer.at(i).clear();
	}
}