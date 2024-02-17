#include <array>
#include <algorithm>
#include <numeric>
#if _WIN32
#include <Windows.h>
#endif // _WIN32

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
#include "db_files.hpp"

extern int vid_width, vid_height;

// can't be constexpr since atan() isn't constexpr for some reason
NO_DISCARD static float FOV_HORZ_TO_VERTICAL(float fovh, float aspect_inv) {
    return 2.0f * atan(tan(fovh / 2) * aspect_inv);
}

NO_DISCARD constexpr static float VID_ASPECT() {
    return (float)vid_width  / (float)vid_height;
}

NO_DISCARD constexpr static float VID_ASPECT_INV() {
    return (float)vid_height / (float)vid_width;
}

NO_DISCARD constexpr static std::string_view GL_ERROR_STR(GLenum err) {
    switch (err) {
    case GL_NO_ERROR:
        return "GL_NO_ERROR";
    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";
    case GL_STACK_OVERFLOW:
        return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW:
        return "GL_STACK_UNDERFLOW";
    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";
    default:
        Com_DPrintln("R_GlErrorStr called with unknown value (err={}}", err);
        return "";
    }
}

static GLenum R_GlCheckError(int line, const char* file) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::string_view s = GL_ERROR_STR(err);
        Com_Println(CON_DEST_ERR, "GL call at {} in {} failed with {}.", line, file, s);
    }
    return err;
}

template<typename Func, typename...Args>
static auto R_GlCallImpl(
    int line, const char* file, Func func, Args... args
) -> typename std::enable_if_t<
    !std::is_same_v<void, decltype(func(args...))>, 
    decltype(func(args...))
> {
    auto a = func(std::forward<Args>(args)...);
    R_GlCheckError(line, file);
    return a;
}

template<typename Func, typename ...Args>
static auto R_GlCallImpl(
    int line, const char* file, Func func, Args... args
) -> typename std::enable_if_t<
    std::is_same_v<void, decltype(func(args...))>,
    GLenum
> {
    func(std::forward<Args>(args)...);
    return R_GlCheckError(line, file);
}

#define GL_CALL(func, ...)                              \
    R_GlCallImpl(__LINE__, __FILE__, func, __VA_ARGS__);

NO_DISCARD constexpr static std::string_view GL_DEBUG_SOURCE_STR(GLenum source) {
    switch (source) {
    case GL_DEBUG_SOURCE_API:
        return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        return "WindowSystem";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        return "ShaderCompiler";
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        return "ThirdParty";
    case GL_DEBUG_SOURCE_APPLICATION:
        return "Application";
    case GL_DEBUG_SOURCE_OTHER:
        return "Other";
    default:
        return "<unknown>";
    };
}

NO_DISCARD constexpr static std::string_view GL_DEBUG_TYPE_STR(GLenum type) {
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        return "Error";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        return "DeprecatedBehaviour";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        return "UndefinedBehaviour";
    case GL_DEBUG_TYPE_PORTABILITY:
        return "Portability";
    case GL_DEBUG_TYPE_PERFORMANCE:
        return "Performance";
    case GL_DEBUG_TYPE_MARKER:
        return "Marker";
    case GL_DEBUG_TYPE_PUSH_GROUP:
        return "PushGroup";
    case GL_DEBUG_TYPE_POP_GROUP:
        return "PopGroup";
    case GL_DEBUG_TYPE_OTHER:
        return "Other";
    default:
        return "<unknown>";
    };
}

NO_DISCARD constexpr static std::string_view GL_DEBUG_SEVERITY_STR(GLenum severity) {
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        return "High";
    case GL_DEBUG_SEVERITY_MEDIUM:
        return "Medium";
    case GL_DEBUG_SEVERITY_LOW:
        return "Low";
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        return "Notification";
    default: 
        return "<unknown>";
    };
}

void APIENTRY R_GlDebugOutput(
    GLenum source, GLenum type, unsigned int id, GLenum severity,
    GLsizei length, const char* message, const void* userParam
) {
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::string_view src = GL_DEBUG_SOURCE_STR(source);
    std::string_view t   = GL_DEBUG_TYPE_STR(type);
    std::string_view sev = GL_DEBUG_SEVERITY_STR(severity);
    
    Com_DPrintln(
        "OpenGL debug message (source={}, type={}, severity={}): {}", 
        src, t, sev, message
    );
}

//static void R_InitCubePrim(INOUT GfxCubePrim& cubePrim);
NO_DISCARD static bool R_CompileShader(
    std::string_view shaderSource, int type, 
    OPTIONAL_OUT std::string* log, OUT unsigned int& shader
);
NO_DISCARD static bool R_LinkShaders(
    vertex_shader_t vertShader, fragment_shader_t fragShader, 
    OPTIONAL_OUT std::string* log, OUT shader_program_t& program
);
static void R_DrawFrameInternal(uint64_t deltaTime);

constexpr float NEAR_PLANE_DEFAULT = 0.1f;
constexpr float FAR_PLANE_DEFAULT  = 100.0f;

//GfxCubePrim r_cubePrim;
GfxFont r_font;
glm::mat4 r_orthoProjection;

float r_nearPlane, r_farPlane;

void R_Init() {
    RB_Init();

    int imgInitFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    if ((IMG_Init(imgInitFlags) & imgInitFlags) != imgInitFlags)
        Com_Errorln("SDL_image init failed.");

#if _DEBUG
    //GLint flags = 0;
    //GL_CALL(glGetIntegerv, GL_CONTEXT_FLAGS, &flags);
    //if ((flags & GL_CONTEXT_FLAG_DEBUG_BIT) == 0)
    //    Com_Errorln("Failed to initialize OpenGL debug context in debug build.");

    GL_CALL(glEnable, GL_DEBUG_OUTPUT);
    GL_CALL(glEnable, GL_DEBUG_OUTPUT_SYNCHRONOUS);
    GL_CALL(glDebugMessageCallback, R_GlDebugOutput, nullptr);
    GL_CALL(glDebugMessageControl, GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif // _DEBUG

    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    GL_CALL(glClearColor, 0.2f, 0.3f, 0.3f, 1.0f);
    GL_CALL(glViewport, 0, 0, vid_width, vid_height);
    r_nearPlane = NEAR_PLANE_DEFAULT;
    r_farPlane = FAR_PLANE_DEFAULT;

    r_orthoProjection = glm::ortho(0.0f, (float)vid_width, 0.0f, (float)vid_height);
    CG_Camera().perspectiveProjection = glm::perspective(FOV_HORZ_TO_VERTICAL(CG_Fov(), VID_ASPECT_INV()), VID_ASPECT(), r_nearPlane, r_farPlane);

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

    assert(R_CreateImage("container.jpg", NULL, NULL, cubePrim.tex));

    glUseProgram(cubePrim.prog.program);
    R_SetUniform(
        cubePrim.prog.program, "uPerspectiveProjection", glm::perspective(
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

void R_WindowResized() {
    r_orthoProjection = glm::ortho(0.0f, (float)vid_width, 0.0f, (float)vid_height);
    CG_Camera().perspectiveProjection = glm::perspective(FOV_HORZ_TO_VERTICAL(CG_Fov(), VID_ASPECT_INV()), VID_ASPECT(), r_nearPlane, r_farPlane);
    GL_CALL(glViewport, 0, 0, vid_width, vid_height); 
}

NO_DISCARD static bool R_CompileShader(
    const std::string& shaderSource, int type, 
    OPTIONAL_OUT std::string* log, OUT unsigned int& shader
) {
    shader = GL_CALL(glCreateShader, type);

    const char* p = shaderSource.c_str();
    GL_CALL(glShaderSource, shader, 1, &p, nullptr);
    GL_CALL(glCompileShader, shader);

    if (log && !log->empty())
        log->clear();
    int success = GL_FALSE;
    GL_CALL(glGetShaderiv, shader, GL_COMPILE_STATUS, &success);
    if (!success && log) {
        GLint len = 0;
        GL_CALL(glGetShaderiv, shader, GL_INFO_LOG_LENGTH, &len);
        log->reserve(len);
        GL_CALL(glGetShaderInfoLog, shader, len, &len, log->data());
    }
    
    return success == GL_TRUE;
}

NO_DISCARD static bool R_LinkShaders(
    vertex_shader_t vertShader, fragment_shader_t fragShader, 
    OPTIONAL_OUT std::string* log, OUT shader_program_t& program
) {
    program = GL_CALL(glCreateProgram);
    GL_CALL(glAttachShader, program, vertShader);
    GL_CALL(glAttachShader, program, fragShader);
    GL_CALL(glLinkProgram, program);

    if(log)
        *log = std::string();
    int success = GL_FALSE;
    GL_CALL(glGetProgramiv, program, GL_LINK_STATUS, &success);
    if(!success && log) {
        GLint len = 0;
        GL_CALL(glGetProgramiv, program, GL_INFO_LOG_LENGTH, &len);
        log->resize(len);
        GL_CALL(glGetProgramInfoLog, program, len, nullptr, log->data());
    }

    return success == GL_TRUE;
}

NO_DISCARD static bool R_CreateShaderProgram(
    const std::string& vertexSource, const std::string& fragmentSource, 
    OPTIONAL_OUT std::string* log, OUT GfxShaderProgram& prog
) {
    prog = GfxShaderProgram{};

    vertex_shader_t v = 0;
    if (!R_CompileShader(vertexSource, GL_VERTEX_SHADER, log, v))
        return false;

    fragment_shader_t f = 0;
    if (!R_CompileShader(fragmentSource, GL_FRAGMENT_SHADER, log, f))
        return false;

    shader_program_t p = 0;
    if (!R_LinkShaders(v, f, log, p))
        return false;

    prog.vertex_shader = v;
    prog.fragment_shader = f;
    prog.program = p;
    return true;
}

NO_DISCARD bool R_CreateImage(
    std::string_view image_name, OPTIONAL_OUT int* width, 
    OPTIONAL_OUT int* height, OUT texture_t& tex
) {
    if(width)
        *width  = 0;
    if(height)
        *height = 0;

    GL_CALL(glGenTextures, 1, &tex);
    GL_CALL(glBindTexture, GL_TEXTURE_2D, tex);

    GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
    GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
    GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SdlSurface surf = SdlSurface(
        IMG_Load(DB_ImagePath(image_name).string().c_str())
    );

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

    GL_CALL(glTexImage2D,
        GL_TEXTURE_2D, 0, GL_RGB, surf->w, surf->h, 
        0, format, GL_UNSIGNED_BYTE, surf->pixels
    );
    GL_CALL(glGenerateMipmap, GL_TEXTURE_2D);

    if (width)
        *width = surf->w;
    if (height)
        *height = surf->h;

    return true;
}

inline static const std::array<GfxSubTexDef, 6> s_subTexDefs = {
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
NO_DISCARD bool R_CreateFont(
    std::string_view font_name, int width, int height, OUT GfxFont& f
) {
    f = GfxFont {};

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

    for (const auto& g : fd.glyphs) {
        f.atlas_width += g.width + 1;
        f.atlas_height = std::max(f.atlas_height, g.height);
    }

    GL_CALL(glUseProgram, r_font.prog.program);

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
        0, 2, GL_FLOAT, GL_FALSE, 
        sizeof(GfxSubTexDef), (void*)offsetof(GfxSubTexDef, x)
    );
    GL_CALL(glVertexAttribPointer,
        1, 2, GL_FLOAT, GL_FALSE, 
        sizeof(GfxSubTexDef), (void*)offsetof(GfxSubTexDef, u)
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

    GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
    GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
            GL_CALL(glTexSubImage2D,
                GL_TEXTURE_2D, 0, x, 0, g.width, g.height, 
                GL_RED, GL_UNSIGNED_BYTE, g.pixels.data()
            );
        
        x += g.width + 1;
    }

    R_SetUniform(f.prog.program, "uOrthoProjection", r_orthoProjection);
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
    const GfxFont& font, std::string_view text, float x, float y, 
    float xscale, float yscale, const glm::vec3& color
) {
    GL_CALL(glEnable, GL_BLEND);
    //GL_CALL(glEnable, GL_CULL_FACE);

    GL_CALL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GL_CALL(glUseProgram, font.prog.program);
    GL_CALL(glBindVertexArray, font.vao);
    GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, font.vbo);
    GL_CALL(glActiveTexture, GL_TEXTURE0);
    GL_CALL(glBindTexture, GL_TEXTURE_2D, font.tex);

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
            glm::vec4 atlasCoord(
                g.atlas_x, g.atlas_y, 
                (float)g.width / (float)font.atlas_width, 
                (float)g.height / (float)font.atlas_height
            );
            R_SetUniform(font.prog.program, "uAtlasCoord", atlasCoord);
        }

        GL_CALL(glDrawArrays, GL_TRIANGLES, 0, 6);

        x += g.advance_x * xscale;
        last_c = c;
        last_g = &g;
    }

    GL_CALL(glDisable, GL_CULL_FACE);
    GL_CALL(glDisable, GL_BLEND);
}

bool R_DrawText(
    std::string_view font_name, std::string_view text, 
    int width, int height, float x, float y,
    float xscale, float yscale, const glm::vec3& color
) {
    GfxFont f;
    if (!R_CreateFont(font_name, width, height, f))
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

    //GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GL_CALL(glClear, GL_COLOR_BUFFER_BIT);

    /*
    glm::vec3 pos = CG_Camera().pos;
    glm::vec3 center = pos + CG_Camera().front;
    glm::vec3 up = CG_Camera().up;
    glm::mat4 view = glm::lookAt(pos, center, up);
    R_SetUniform(r_cubePrim.prog.program, "uView", view);
    */

    R_DrawText(
        r_font, "Testing 123...", 
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
    IMG_Quit();
    RB_Shutdown();
}