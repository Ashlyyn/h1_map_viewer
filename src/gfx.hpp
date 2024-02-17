#pragma once

#include <string_view>
#include <memory>
#include <filesystem>

#include "com_defs.hpp"
#include "gfx_defs.hpp"

void R_Init();
void R_DrawFrame(uint64_t deltaTime);
void R_WindowResized();
NO_DISCARD bool R_CreateImage(
	std::string_view image_name, OPTIONAL_OUT int* width, 
	OPTIONAL_OUT int* height, OUT texture_t& tex
);
NO_DISCARD bool R_CreateFont(
	std::string_view font_name, int width, int height, OUT GfxFont& f
);
void R_Shutdown();