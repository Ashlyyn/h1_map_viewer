#pragma once

#include <array>
#include <filesystem>
#include <span>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "com_defs.hpp"
#include "gfx_defs.hpp"

struct GlyphDef {
	char  c;
	int   width, height;
	int   left;
	int   top;
	int   advance_x, advance_y;
    float atlas_x, atlas_y;
	std::vector<unsigned char> pixels;
};

struct FontDef {
    int atlas_width, atlas_height;
    GfxShaderProgram prog;
    vao_t     vao;
    vbo_t     vbo;
    texture_t tex;

	inline bool AddGlyph(char c, const GlyphDef& g) {
		if (c < 32 || c > 127)
			return false;
		
		glyphs[c - 32] = g;
		return true;
	}

	inline bool GetGlyph(char c, OUT GlyphDef*& g) {
		if (c < 32 || c > 127) {
            g = nullptr;
			return false;
		}

		g = &glyphs.at(c - 32);
		return true;
	}

    inline bool GetGlyph(char c, OUT const GlyphDef*& g) const {
        if (c < 32 || c > 127) {
            g = nullptr;
            return false;
        }

        g = &glyphs.at(c - 32);
        return true;
    }

    inline bool RemoveGlyph(char c) {
        if (c < 32 || c > 127)
            return false;

        glyphs[c - 32] = GlyphDef{};
        return true;
    }

	inline std::span<GlyphDef> Glyphs() {
		return std::span(glyphs);
	}

    void Take(FontDef&& other) {
        glyphs = other.glyphs;
        atlas_width = other.atlas_width;
        atlas_height = other.atlas_height;
        prog = std::move(other.prog);
        vao = other.vao;
        vbo = other.vbo;
        tex = other.tex;

        other.atlas_height = 0;
        other.atlas_width = 0;
        other.vao = 0;
        other.vbo = 0;
        other.tex = 0;
    }

    FontDef() {
        atlas_width = 0;
        atlas_height = 0;
        vao = 0;
        vbo = 0;
        tex = 0;
    }

    FontDef(const FontDef&) = delete;
    FontDef(FontDef&& other) {
        Take(std::move(other));
    }

    FontDef& operator=(const FontDef&) = delete;
    FontDef& operator=(FontDef&& other) {
        Take(std::move(other));
        return *this;
    }

    inline ~FontDef() {
        glDeleteTextures(1, &tex);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &vao);
    }

private:
	std::array<GlyphDef, 95> glyphs;
}; 


void Font_Init();
NO_DISCARD bool Font_Load(
	std::string_view font_name, int width, int height, OUT FontDef& fd
);
void Font_Shutdown();