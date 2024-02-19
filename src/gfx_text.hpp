#pragma once

#include <string_view>
#include <string>

#include "com_defs.hpp"
#include "gfx_defs.hpp"


NO_DISCARD bool R_CreateFont(
	std::string_view font_name, int width, int height, OUT GfxFont& f
);
void R_DrawText(
 	int localClientNum, OPTIONAL_IN const GfxFont* font, std::string_view text,
	float x, float y, float xscale, float yscale, const glm::vec3& color, 
	bool right = false
);
bool R_DrawText(
	int localClientNum, std::string_view font_name, std::string_view text,
	int width, int height, float x, float y, float xscale, float yscale, 
	const glm::vec3& color, bool right, OPTIONAL_OUT GfxFont* f
);
bool R_AddTextDraw(
	int localClientNum, GfxFont* font, std::string text, float x, float y, 
	float xscale, float yscale, glm::vec3 color, bool active, bool right, 
	OUT size_t& id
);

bool R_UpdateTextDraw(int localClientNum, size_t id, std::string text);
bool R_ActivateTextDraw(int localClientNum, size_t id, bool active);
bool R_RemoveTextDraw(int localClientNum, size_t id);