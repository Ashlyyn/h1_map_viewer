#include <array>
#include <algorithm>
#include <numeric>

#include <cstdio>
#include <cassert>

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/gtc/matrix_transform.hpp>

#include "com_print.hpp"
#include "fs_files.hpp"
#include "gfx.hpp"
#include "gfx_backend.hpp"
#include "gfx_uniform.hpp"
#include "cg_cgame.hpp"

extern int vid_width, vid_height;

//static void R_InitCubePrim(INOUT GfxCubePrim& cubePrim);
static bool R_CompileShader(
    std::string_view shaderSource, int type, 
    OPTIONAL_OUT std::string* log, OUT unsigned int& shader
);
static bool R_LinkShaders(
    vertex_shader_t vertShader, fragment_shader_t fragShader, 
    OPTIONAL_OUT std::string* log, OUT shader_program_t& program
);
static void R_DrawFrameInternal(uint64_t deltaTime);

//GfxCubePrim r_cubePrim;
GfxFont r_font;

void R_Init() {
    RB_Init();

#if _DEBUG
    //glEnable(GL_DEBUG_OUTPUT);
#endif // _DEBUG

    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, vid_width, vid_height);

    assert(R_CreateFont("consola.ttf", 0, 48, r_font));

    //R_InitCubePrim(r_cubePrim);
     
    //glEnable(GL_POINT_SMOOTH);
    //glPointSize(4);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
}

/*
static void R_InitCubePrim(INOUT GfxCubePrim& cubePrim) {
    std::string vertSource = FS_ReadFileText("vs.glsl");
    Com_DPrintln(vertSource);

    std::string errorLog;

    if (!R_CompileShader(
        vertSource, GL_VERTEX_SHADER, &errorLog, cubePrim.prog.vertex_shader
    )) {
        Com_Error(errorLog);
    }

    std::string fragSource = FS_ReadFileText("fs.glsl");
    Com_DPrintln(fragSource);

    if (!R_CompileShader(
        fragSource, GL_FRAGMENT_SHADER, &errorLog, cubePrim.prog.fragment_shader
    )) {
        Com_Errorln(errorLog);
    }

    if (!R_LinkShaders(
        cubePrim.prog.vertex_shader, cubePrim.prog.fragment_shader, 
        &errorLog, cubePrim.prog.program
    )) {
        Com_Errorln(errorLog);
    }

    glGenVertexArrays(1, &cubePrim.vao);
    glBindVertexArray(cubePrim.vao);

    glGenBuffers(1, &cubePrim.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, cubePrim.vbo);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(GfxCubePrim::vertices), 
        GfxCubePrim::vertices, GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GfxCubePrim::vertices), (void*)0
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(*GfxCubePrim::vertices), 
        (void*)(3 * sizeof(*GfxCubePrim::vertices))
    );
    glEnableVertexAttribArray(1);

    assert(R_LoadImage("container.jpg", NULL, NULL, cubePrim.tex));

    glUseProgram(cubePrim.prog.program);
    R_SetUniform(
        cubePrim.prog.program, "uProjection", glm::perspective(
            glm::radians(74.0f), (float)vid_height / (float)vid_width, 0.1f, 100.0f
        )
    );
    R_SetUniform(cubePrim.prog.program, "uContainerTex", 0u);
}
*/

void R_DrawFrame(uint64_t deltaTime) {
    RB_BeginFrame();

    R_DrawFrameInternal(deltaTime);

    RB_EndFrame();
}

void R_ResizeWindow() {
    //  R_SetUniform(r_cubePrim.prog.program, "uProjection", 
    //      glm::perspective(
    //          glm::radians(74.0f), (float)vid_height / (float)vid_width, 0.1f, 100.0f
    //      )
    // );
    glViewport(0, 0, vid_width, vid_height); 
}

static bool R_CompileShader(
    std::string_view shaderSource, int type, 
    OPTIONAL_OUT std::string* log, OUT unsigned int& shader
) {
    shader = glCreateShader(type);

    auto* p = shaderSource.data();
    glShaderSource(shader, 1, &p, NULL);
    glCompileShader(shader);

    if (log && !log->empty())
        log->clear();
    int success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success && log) {
        GLint len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        log->reserve(len);
        glGetShaderInfoLog(shader, len, &len, log->data());
    }
    
    return success == GL_TRUE;
}

static bool R_LinkShaders(
    vertex_shader_t vertShader, fragment_shader_t fragShader, 
    OPTIONAL_OUT std::string* log, OUT shader_program_t& program
) {
    program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    if(log)
        *log = std::string();
    int success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success && log) {
        GLint len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        log->resize(len);
        glGetProgramInfoLog(program, len, NULL, log->data());
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return success == GL_TRUE;
}

bool R_LoadImage(
    std::filesystem::path path, OPTIONAL_OUT int* width, 
    OPTIONAL_OUT int* height, OUT texture_t& tex
) {
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SdlSurface surf = SdlSurface(IMG_Load(path.string().c_str()));
    if (!surf)
        return false;

    if (!surf->pixels)
        return false;

    if (surf->w < 1 || surf->h < 1)
        return false;

    image_format_t format = 0;
    switch (surf->format->format) {
    case SDL_PIXELFORMAT_RGB24:
        format = GL_RGB;
        break;
    case SDL_PIXELFORMAT_RGBA32:
        format = GL_RGBA;
        break;
    default:
        assert(false);
    }

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, surf->w, surf->h, 
        0, format, GL_UNSIGNED_BYTE, surf->pixels
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    if (width)
        *width = surf->w;
    if (height)
        *height = surf->h;

    return true;
}

inline static const std::array<GfxSubTexDef, 6> g_subTexDefs = {
    GfxSubTexDef { .x = 0, .y = 0, .u = 0, .v = 1 },
    GfxSubTexDef { .x = 0, .y = 1, .u = 0, .v = 0 },
    GfxSubTexDef { .x = 1, .y = 1, .u = 1, .v = 0 },
    GfxSubTexDef { .x = 0, .y = 0, .u = 0, .v = 1 },
    GfxSubTexDef { .x = 1, .y = 1, .u = 1, .v = 0 },
    GfxSubTexDef { .x = 1, .y = 0, .u = 1, .v = 1 }
};

// ============================================================================
// DON'T TOUCH ANY OF THIS. DON'T EVEN LOOK AT IT THE WRONG WAY. LEAVE IT ALONE
// AND BE HAPPY YOU DIDN'T SPEND TEN HOURS GETTING IT TO WORK.
bool R_CreateFont(
    std::filesystem::path font_path, 
    int width, int height, OUT GfxFont& f
) {
    FontDef fd;
    if (!Font_Load(font_path, width, height, fd))
        return false;

    for (const auto& g : fd.glyphs) {
        f.atlas_width += g.width + 1;
        f.atlas_height = std::max(f.atlas_height, g.height);
    }

    glGenVertexArrays(1, &f.vao);
    glBindVertexArray(f.vao);
    glGenBuffers(1, &f.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, f.vbo);
    glBufferData(
        GL_ARRAY_BUFFER, 
        g_subTexDefs.size() * sizeof(*g_subTexDefs.data()), 
        g_subTexDefs.data(), 
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GfxSubTexDef), 0);
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, 
        sizeof(GfxSubTexDef), (void*)(2 * sizeof(float))
    );
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glGenTextures(1, &f.tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, f.tex);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RED, f.atlas_width, 
        f.atlas_height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL
    );
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int x = 0;
    for (char c = 32; c < 127; c++) {
        const auto& g = fd.glyphs[c - 32];
        if (g.advance_x == 0)
            continue;

        GfxGlyph gg;
        gg.width = g.width;
        gg.height = g.height;
        gg.advance_x = g.advance_x >> 6;
        gg.advance_y = 0;
        gg.left = g.left;
        gg.top = g.top;
        gg.atlas_x = (float)x / (float)f.atlas_width;
        gg.atlas_y = 0;
        f.glyphs[c - 32] = gg;

        if(g.pixels.size() > 0)
            glTexSubImage2D(
                GL_TEXTURE_2D, 0, x, 0, g.width, g.height, 
                GL_RED, GL_UNSIGNED_BYTE, g.pixels.data()
            );
        
        x += g.width + 1;
    }

    if (!R_CompileShader(
        FS_ReadFileText("text.vs.glsl"), 
        GL_VERTEX_SHADER, NULL, f.prog.vertex_shader
    )) {
        Font_Unload(fd);
        return false;
    }

    if (!R_CompileShader(
        FS_ReadFileText("text.fs.glsl"),
        GL_FRAGMENT_SHADER, NULL, f.prog.fragment_shader
    )) {
        Font_Unload(fd);
        return false;
    }

    if (!R_LinkShaders(
        f.prog.vertex_shader, f.prog.fragment_shader, NULL, f.prog.program
    )) {
        Font_Unload(fd);
        return false;
    }

    f.projection = glm::ortho(0.0f, (float)vid_width, 0.0f, (float)vid_height);
    glUseProgram(f.prog.program);
    R_SetUniform(f.prog.program, "uProjection", f.projection);
    R_SetUniform(f.prog.program, "uTex", 0);
    glUseProgram(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Font_Unload(fd);

    return true;
}
// ============================================================================

void R_UnloadFont(GfxFont& font) {
    glDeleteShader(font.prog.vertex_shader);
    glDeleteShader(font.prog.fragment_shader);
    glDeleteProgram(font.prog.program);
    glDeleteTextures(1, &font.tex);
    glDeleteBuffers(1, &font.vao);
    glDeleteBuffers(1, &font.vbo);
}

std::array<GfxSubTexDef, 1024 * 6> g_textCoords;

void R_DrawText(
    const GfxFont& font, std::string_view text, float x, float y, 
    float xscale, float yscale, const glm::vec3& color
) {
    glEnable(GL_BLEND);
    glUseProgram(font.prog.program);
    glBindVertexArray(font.vao);
    glBindBuffer(GL_ARRAY_BUFFER, font.vbo);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font.tex);

    R_SetUniform(font.prog.program, "uTextColor", color);

    char last_c = '\0';
    float last_w = 0, last_h = 0;
    const GfxGlyph* last_g = NULL;

    for (auto c : text) {
        // Make sure c != last_c so that g gets bound to a valid glyph instead
        // of getting bound to last_g and derefing NULL on the first iteration
        // of the loop.
        assert(c != '\0');

        // Reuse the last glyph if it's going to be rendered again.
        const auto& g = c == last_c ? *last_g : font.glyphs[c - 32];

        // Skip rendering a glyph if it has no advance
        if (g.advance_x == 0)
            continue;

        // Scale the width and height and update the last ones, or reuse the 
        // last ones if the same glyph is being rendered.
        float w = c == last_c ? last_w : g.width  * xscale;
        float h = c == last_c ? last_h : g.height * yscale;
        last_w = w;
        last_h = h;

        // Glyphs can have no width or height because they have no pixel data
        // but a nonzero advance (i.e., space). Skip rendering on those and
        // just update the cursor position.
        if (g.width == 0 || g.height == 0) {
            x += g.advance_x * xscale;
            last_c = c;
            last_g = &g;
            continue;
        }

        float xpos = x + g.left;
        float ypos = y - (g.height - g.top);

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(xpos, ypos, 0.0f));
        model = glm::scale(model, glm::vec3(w, h, 0.0f));
        R_SetUniform(font.prog.program, "uModel", model);

        // If the same glyph is being rendered again, the coord uniform doesn't
        // need to be updated.
        if (c != last_c) {
            glm::vec4 coord(
                g.atlas_x, g.atlas_y, 
                g.width / font.atlas_width, g.height / font.atlas_height
            );
            R_SetUniform(font.prog.program, "uCoord", coord);
        }

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += g.advance_x * xscale;
        last_c = c;
        last_g = &g;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    glDisable(GL_BLEND);
}

bool R_DrawText(
    std::filesystem::path font_path, std::string_view text, 
    int width, int height, float x, float y,
    float xscale, float yscale, const glm::vec3& color
) {
    GfxFont f;
    if (!R_CreateFont(font_path, width, height, f))
        return false;

    R_DrawText(f, text, x, y, xscale, yscale, color);
    return true;
}

/*
void R_DrawCube(const glm::vec3& pos, float angle, texture_t tex) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glBindVertexArray(r_cubePrim.vao);
    glUseProgram(r_cubePrim.prog.program);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0, 0.3, 0.5));
    R_SetUniform(r_cubePrim.prog.program, "uModel", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUseProgram(0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
*/

static void R_DrawFrameInternal(uint64_t deltaTime) {
    /*
    static const std::array<glm::vec3, 10> cubePositions = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    */

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*
    glm::vec3 pos = CG_Camera().pos;
    glm::vec3 center = pos + CG_Camera().front;
    glm::vec3 up = CG_Camera().up;
    glm::mat4 view = glm::lookAt(pos, center, up);
    R_SetUniform(r_cubePrim.prog.program, "uView", view);
    */

    R_DrawText(
        r_font, "!Testing 123...", 
        25.0, 25.0, 1.0, 1.0, 
        glm::vec3(0.5, 0.8f, 0.2f)
    );

    /*
    for (int i = 0; i < cubePositions.size(); i++) {
        R_DrawCube(cubePositions[i], 20.0f * (float)i, r_cubePrim.tex);
    }
    */
}

void R_Shutdown() {
    R_UnloadFont(r_font);
    RB_Shutdown();
}