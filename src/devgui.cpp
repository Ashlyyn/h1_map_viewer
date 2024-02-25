#include "devgui.hpp"

#include <array>

#include "cl_client.hpp"
#include "gfx_text.hpp"
#include "in_input.hpp"

struct dgl_t {
	size_t promptDrawId;
	std::string buffer;
};

std::array<dgl_t, MAX_LOCAL_CLIENTS> s_dgl;

dgl_t& DevGui_GetLocalClientLocals(int localClientNum) {
	return s_dgl.at(localClientNum);
}

void DevGui_Init() {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		dgl_t& dgl = DevGui_GetLocalClientLocals(i);
		dgl.buffer = "> ";

		R_AddTextDraw(
			i, nullptr, dgl.buffer,
			0.1f, 0.9f, 1.0f, 1.0f, glm::vec3(0.9f, 0.2f, 0.2f),
			false, false, dgl.promptDrawId
		);
	}
}

bool DevGui_HasText(int localClientNum) {
	return DevGui_GetLocalClientLocals(localClientNum).buffer.contains('\n');
}

std::string DevGui_TakeText(int localClientNum) {
	dgl_t& dgl = DevGui_GetLocalClientLocals(localClientNum);

	size_t pos = dgl.buffer.find('\n');
	size_t len = pos - 2;
	std::string input = dgl.buffer.substr(2, len);
	dgl.buffer.erase(2, len + 1);
	return input;
}

void DevGui_Frame() {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		dgl_t& dgl = DevGui_GetLocalClientLocals(i);
		if (CL_KeyFocus(i) != KF_DEVGUI) {
			R_ActivateTextDraw(i, dgl.promptDrawId, false);
			continue;
		}

 		for (auto k : IN_Key_AllPressedOnCurrentFrame(i)) {
			char c = IN_Key_Char(i, k);
			if (c != 0 && k != SDLK_BACKQUOTE)
				dgl.buffer.push_back(c);
			else if (k == SDLK_BACKSPACE && dgl.buffer.length() > 2)
				dgl.buffer.pop_back();
			else if (k == SDLK_DELETE)
				dgl.buffer = "> ";
		}

		R_UpdateTextDraw(i, dgl.promptDrawId, dgl.buffer);
		R_ActivateTextDraw(i, dgl.promptDrawId, true);
	}
}

void DevGui_Shutdown() {
	for (int i = 0; i < MAX_LOCAL_CLIENTS; i++) {
		dgl_t& dgl = DevGui_GetLocalClientLocals(i);
		R_RemoveTextDraw(i, dgl.promptDrawId);
		dgl.buffer.clear();
	}
}