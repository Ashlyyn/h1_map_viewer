#pragma once

#include <array>
#include <filesystem>
#include <span>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "com_defs.hpp"

struct GlyphDef {
	char c;
	int  width;
	int  height;
	int  left;
	int  top;
	int  advance_x;
	int  advance_y;
	std::vector<unsigned char> pixels;
};

struct FontDef {
	inline bool AddGlyph(char c, const GlyphDef& g) {
		if (c < 32 || c > 127)
			return false;
		
		glyphs[c - 32] = g;
		return true;
	}

	inline bool GetGlyph(char c, OUT GlyphDef& g) const {
		if (c < 32 || c > 127) {
			g = GlyphDef{};
			return false;
		}

		g = glyphs.at(c);
		return true;
	}

	inline std::span<GlyphDef> Glyphs() {
		return std::span(glyphs);
	}

private:
	std::array<GlyphDef, 95> glyphs;
};

void Font_Init();
NO_DISCARD bool Font_Load(
	std::string_view font_name, int width, int height, OUT FontDef& fd
);
void Font_Shutdown();