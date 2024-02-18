#pragma once

#include <vector>
#include <array>
#include <filesystem>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "com_defs.hpp"

struct GlyphDef {
	int width;
	int height;
	int left;
	int top;
	int advance_x;
	int advance_y;
	std::vector<unsigned char> pixels;
};

struct FontDef {
	std::array<GlyphDef, 95> glyphs;
};

void Font_Init();
NO_DISCARD bool Font_Load(
	std::string_view font_name, int width, int height, OUT FontDef& fd
);
void Font_Shutdown();