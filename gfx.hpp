#pragma once

#include <memory>
#include <filesystem>

#include "com_defs.hpp"
#include "gfx_defs.hpp"

void R_Init();
void R_DrawFrame(uint64_t deltaTime);
void R_ResizeWindow();
bool R_LoadImage(
	std::filesystem::path path, OPTIONAL_OUT int* width, 
	OPTIONAL_OUT int* height, OUT texture_t& tex
);
bool R_CreateFont(
	std::filesystem::path font_path, int width, int height, OUT GfxFont& f
);
void R_Shutdown();