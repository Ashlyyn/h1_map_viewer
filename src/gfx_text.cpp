#include "gfx_text.hpp"

#include <array>

#include <glm/gtc/matrix_transform.hpp>

#include "cg_cgame.hpp"
#include "db_files.hpp"
#include "gfx_shaders.hpp"
#include "gfx_uniform.hpp"
#include "sys.hpp"

extern int vid_width, vid_height;
GfxFont r_defaultFont;

inline static const std::array<GfxSubTexDef, 6> s_subTexDefs = {
    GfxSubTexDef {.x = 0, .y = 0, .u = 0, .v = 1 },
    GfxSubTexDef {.x = 0, .y = 1, .u = 0, .v = 0 },
    GfxSubTexDef {.x = 1, .y = 1, .u = 1, .v = 0 },
    GfxSubTexDef {.x = 0, .y = 0, .u = 0, .v = 1 },
    GfxSubTexDef {.x = 1, .y = 1, .u = 1, .v = 0 },
    GfxSubTexDef {.x = 1, .y = 0, .u = 1, .v = 1 }
};

// ============================================================================
// DON'T TOUCH ANY OF THIS. DON'T EVEN LOOK AT IT THE WRONG WAY. LEAVE IT ALONE
// AND BE HAPPY YOU DIDN'T SPEND TEN HOURS GETTING IT TO WORK.
NO_DISCARD bool R_CreateFont(
    std::string_view font_name, int width, int height, OUT GfxFont& f
) {
    f = GfxFont{};

    FontDef fd;
    if (!Font_Load(font_name, width, height, fd))
        return false;

    if (!R_CreateShaderProgram(
        DB_LoadShader("text.vs.glsl"),
        DB_LoadShader("text.fs.glsl"),
        NULL, f.prog)
    ) {
        return false;
    }

    for (const auto& g : fd.Glyphs()) {
        f.atlas_width += g.width + 1;
        f.atlas_height = std::max(f.atlas_height, g.height);
    }

    GL_CALL(glUseProgram, r_defaultFont.prog.program);

    GL_CALL(glGenVertexArrays, 1, &f.vao);
    GL_CALL(glBindVertexArray, f.vao);

    GL_CALL(glGenBuffers, 1, &f.vbo);
    GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, f.vbo);
    GL_CALL(glBufferData,
        GL_ARRAY_BUFFER,
        s_subTexDefs.size() * sizeof(*s_subTexDefs.data()),
        s_subTexDefs.data(),
        GL_STATIC_DRAW
    );
    GL_CALL(glVertexAttribPointer,
        0, 2, GL_FLOAT, (GLboolean)GL_FALSE,
        (GLsizei)sizeof(GfxSubTexDef), (void*)offsetof(GfxSubTexDef, x)
    );
    GL_CALL(glVertexAttribPointer,
        1, 2, GL_FLOAT, (GLboolean)GL_FALSE,
        (GLsizei)sizeof(GfxSubTexDef), (void*)offsetof(GfxSubTexDef, u)
    );
    GL_CALL(glEnableVertexAttribArray, 0);
    GL_CALL(glEnableVertexAttribArray, 1);

    GL_CALL(glGenTextures, 1, &f.tex);
    GL_CALL(glActiveTexture, GL_TEXTURE0);

    GL_CALL(glBindTexture, GL_TEXTURE_2D, f.tex);
    GL_CALL(glTexImage2D,
        GL_TEXTURE_2D, 0, GL_RED, f.atlas_width,
        f.atlas_height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr
    );

    GLint unpackAlign = 0;
    GL_CALL(glGetIntegerv, GL_UNPACK_ALIGNMENT, &unpackAlign);
    GL_CALL(glPixelStorei, GL_UNPACK_ALIGNMENT, 1);

    GL_CALL(glTexParameteri, 
        GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE
    );
    GL_CALL(glTexParameteri, 
        GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE
    );
    GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int x = 0;
    for(const auto& g : fd.Glyphs()) {
        if (g.advance_x == 0)
            continue;

        GfxGlyph gg;
        gg.width     = g.width;
        gg.height    = g.height;
        gg.advance_x = g.advance_x >> 6;
        gg.advance_y = 0;
        gg.left      = g.left;
        gg.top       = g.top;
        gg.atlas_x   = (float)x / (float)f.atlas_width;
        gg.atlas_y   = 0;
        f.AddGlyph(g.c, gg);

        if (g.pixels.size() > 0)
            GL_CALL(glTexSubImage2D,
                GL_TEXTURE_2D, 0, x, 0, g.width, g.height,
                GL_RED, GL_UNSIGNED_BYTE, g.pixels.data()
            );

        x += g.width + 1;
    }

    R_SetUniform(f.prog.program, "uTex", 0);

    GL_CALL(glPixelStorei, GL_UNPACK_ALIGNMENT, unpackAlign);
    GL_CALL(glBindTexture, GL_TEXTURE_2D, 0);
    GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
    GL_CALL(glBindVertexArray, 0);
    GL_CALL(glUseProgram, 0);

    return true;
}
// ============================================================================

void R_DrawText(
    int localClientNum, OPTIONAL_IN const GfxFont* font, std::string_view text, 
    float x, float y, float xscale, float yscale, const glm::vec3& color, 
    bool right
) {
    if (font == nullptr)
        font = &r_defaultFont;

    x *= vid_width;
    y *= vid_height;

    float firstX = x;

    GL_CALL(glUseProgram, font->prog.program);
    GL_CALL(glEnable, GL_BLEND);
    GL_CALL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GL_CALL(glBindVertexArray, font->vao);
    GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, font->vbo);
    GL_CALL(glActiveTexture, GL_TEXTURE0);
    GL_CALL(glBindTexture, GL_TEXTURE_2D, font->tex);

    R_SetUniform(font->prog.program, "uTextColor", color);

    cg_t& cg = CG_GetLocalClientGlobals(localClientNum);

    R_SetUniform(
        font->prog.program, "uOrthoProjection", cg.camera.orthoProjection
    );

    char            last_c = '\0';
    float           last_w = 0, last_h = 0;
    const GfxGlyph* last_g = nullptr;

    int i = right ? (int)text.size() - 1 : 0;
    const int end = right ? -1 : (int)text.size();

    while (i != end) {
        char c = text.at(i);
        // Make sure c != last_c so that g gets bound to a valid glyph instead
        // of getting bound to last_g and derefing NULL on the first iteration
        // of the loop.
        assert(c != '\0');

        if (c == '\n') {
            y -= font->atlas_height;
            x = firstX;
            right ? i-- : i++;
            continue;
        }

        if (c < 32 || c > 127)
            c = '#';

        // Reuse the last glyph if it's going to be rendered again.
        const GfxGlyph* g = nullptr;
        if (c == last_c)
            g = last_g;
        else
            font->GetGlyph(c, g);

        // If the glyph couldn't be retrieved, use '#' as a placeholder.
        if (g == nullptr)
            font->GetGlyph('#', g);

        // If '#' couldn't be retrieved either, just move on to the next char.
        if (g == nullptr) {
            right ? i-- : i++;
            continue;
        }

        // Skip rendering a glyph if it has no advance
        if (g->advance_x == 0) {
            right ? i-- : i++;
            continue;
        }

        // Scale the width and height and update the last ones, or reuse the 
        // last ones if the same glyph is being rendered.
        float w = c == last_c ? last_w
            : g->width * xscale;
        float h = c == last_c ? last_h
            : g->height * yscale;

        last_w = w;
        last_h = h;

        // Glyphs can have no width or height because they have no pixel data
        // but a nonzero advance (i.e., space). Skip rendering on those and
        // just update the cursor position.
        if (g->width == 0 || g->height == 0) {
            if (right)
                x -= g->advance_x * xscale;
            else
                x += g->advance_x * xscale;
            last_c = c;
            last_g = g;
            right ? i-- : i++;
            continue;
        }

        float xpos = x + g->left;
        float ypos = y - (g->height - g->top);

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(xpos, ypos, 0.0f));
        model = glm::scale(model, glm::vec3(w, h, 0.0f));
        R_SetUniform(font->prog.program, "uModel", model);

        // If the same glyph is being rendered again, the coord uniform doesn't
        // need to be updated.
        if (c != last_c) {
            glm::vec4 atlasCoord(
                g->atlas_x, g->atlas_y,
                (float)g->width  / (float)font->atlas_width,
                (float)g->height / (float)font->atlas_height
            );
            R_SetUniform(font->prog.program, "uAtlasCoord", atlasCoord);
        }

        GL_CALL(glDrawArrays, GL_TRIANGLES, 0, 6);

        if (right)
            x -= g->advance_x * xscale;
        else
            x += g->advance_x * xscale;
        last_c = c;
        last_g = g;
        right ? i-- : i++;
    }

    GL_CALL(glBindTexture, GL_TEXTURE_2D, 0);
    GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
    GL_CALL(glBindVertexArray, 0);
    GL_CALL(glUseProgram, 0);
    GL_CALL(glDisable, GL_BLEND);
}

bool R_DrawText(
    int localClientNum, std::string_view font_name, std::string_view text, 
    int width, int height, float x, float y, float xscale, float yscale, 
    const glm::vec3& color, bool right, OPTIONAL_OUT GfxFont* f
) {
    if (f != nullptr)
        *f = GfxFont{};

    GfxFont ff;
    if (!R_CreateFont(font_name, width, height, ff))
        return false;

    R_DrawText(localClientNum, &ff, text, x, y, xscale, yscale, color, right);

    if (f != nullptr)
        *f = std::move(ff);

    return true;
}

std::array<std::array<GfxTextDraw, 256>, MAX_LOCAL_CLIENTS> r_textDraws;

bool R_GetTextDraw(int localClientNum, size_t id, OUT GfxTextDraw*& draw) {
    draw = nullptr;
    assert(id < r_textDraws.at(localClientNum).size());

    draw = &r_textDraws.at(localClientNum).at(id);
    assert(draw);
    return !draw->free;
}


bool R_FindFreeTextDraw(int localClientNum, OUT size_t& index, OUT GfxTextDraw*& draw) {
    index = (size_t)-1;
    draw = nullptr;

    for (int i = 0; i < (int)r_textDraws.at(localClientNum).size(); i++) {
        R_GetTextDraw(localClientNum, i, draw);
        if (draw->free) {
            index = i;
            return true;
        }
    }

    return false;
}

bool R_AddTextDraw(
    int localClientNum, GfxFont* font, std::string text, float x, float y, 
    float xscale, float yscale, glm::vec3 color, bool active, bool right, 
    OUT size_t& id
) {
    GfxTextDraw* d = nullptr;
    if (!R_FindFreeTextDraw(localClientNum, id, d))
        return false;

    d->free = false;
    d->font = font;
    d->text = text;
    d->x = x;
    d->y = y;
    d->xscale = xscale;
    d->yscale = yscale;
    d->color = color;
    d->active = active;
    d->right = right;

    return true;
}

bool R_UpdateTextDraw(int localClientNum, size_t id, std::string text) {
    GfxTextDraw* d = nullptr;
    if (!R_GetTextDraw(localClientNum, id, d))
        return false;

    d->text = text;
    return true;
}

bool R_ActivateTextDraw(int localClientNum, size_t id, bool active) {
    GfxTextDraw* d = nullptr;
    R_GetTextDraw(localClientNum, id, d);
    bool wasActive = d->active;
    d->active = active;
    return wasActive;
}

bool R_RemoveTextDraw(int localClientNum, size_t id) {
    GfxTextDraw* d = nullptr;
    assert(R_GetTextDraw(localClientNum, id, d));

    if (d->free == true)
        return false;

    d->free = true;
    d->active = false;
    return true;
}

void R_ClearTextDraws(int localClientNum) {
    for (auto& c : r_textDraws.at(localClientNum)) {
        c.active = false;
        c.free = true;
    }
}

void R_DrawTextDraws(int localClientNum) {
    for (const auto& c : r_textDraws.at(localClientNum)) {
        if (!c.free && c.active) {
            R_DrawText(
                localClientNum, c.font, c.text, c.x, c.y, 
                c.xscale, c.yscale, c.color, c.right
            );
        }
    }
}
