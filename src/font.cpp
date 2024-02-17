#include "font.hpp"
#include "com_print.hpp"
#include "db_files.hpp"

static FT_Library s_ft;

void Font_Init() {
	if (FT_Init_FreeType(&s_ft))
		Com_Errorln("FreeType init failed.");
}

bool Font_Load(
	std::string_view font_name, int width, int height, OUT FontDef& fd
) {
	fd = FontDef{};

	std::vector<std::byte> font_buf = DB_LoadFont(font_name);
	if (font_buf.empty()) {
		Com_Println(CON_DEST_ERR, "Failed to open font '{}'.", font_name);
		return false;
	}

	FT_Face face;
	if (FT_New_Memory_Face(s_ft, (FT_Byte*)font_buf.data(), font_buf.size(), 0, &face)) {
		Com_Println(CON_DEST_ERR, "Failed to create font '{}'.", font_name);
		return false;
	}

	FT_Set_Pixel_Sizes(face, width, height);

	for (char c = 32; c < 127; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			Com_DPrintln(
				"Failed to load glyph '{}' from font {}.", 
				c, font_name
			);

		GlyphDef glyph;
		glyph.width     = face->glyph->bitmap.width;
		glyph.height    = face->glyph->bitmap.rows;
		glyph.left      = face->glyph->bitmap_left;
		glyph.top       = face->glyph->bitmap_top;
		glyph.advance_x = face->glyph->advance.x;
		glyph.advance_y = face->glyph->advance.y;
		glyph.pixels    = std::vector<unsigned char>(glyph.width * glyph.height);
		memcpy(
			glyph.pixels.data(), face->glyph->bitmap.buffer,
			glyph.pixels.size() * sizeof(*glyph.pixels.data())
		);
		fd.glyphs[c - 32] = glyph;
	}

	FT_Done_Face(face);
	return true;
}

void Font_Shutdown() {
	FT_Done_FreeType(s_ft);
}