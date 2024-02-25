#pragma once

#include <string>
#include <string_view>

#include "com_defs.hpp"
#include "gfx_defs.hpp"


NO_DISCARD bool R_CreateTextureAtlas(INOUT FontDef& f);
void R_DrawText(
 	int localClientNum, OPTIONAL_IN const FontDef* font, std::string_view text,
	float x, float y, float xscale, float yscale, const glm::vec3& color, 
	bool right = false
);
bool R_AddTextDraw(
	int localClientNum, FontDef* font, std::string text, float x, float y, 
	float xscale, float yscale, glm::vec3 color, bool active, bool right, 
	OUT size_t& id
);

bool R_UpdateTextDraw(int localClientNum, size_t id, std::string text);
bool R_ActivateTextDraw(int localClientNum, size_t id, bool active);
bool R_RemoveTextDraw(int localClientNum, size_t id);