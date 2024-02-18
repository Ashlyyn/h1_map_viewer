#include <array>
#include <algorithm>
#include <numeric>
#include <unordered_map>

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
#include "gfx_text.hpp"
#include "cg_cgame.hpp"
#include "db_files.hpp"
#include "sys.hpp"

extern int vid_width, vid_height;
extern void R_DrawTextDraws();
extern void R_ClearTextDraws();

void GLAPIENTRY R_GlDebugOutput(
    GLenum source, GLenum type, unsigned int id, GLenum severity,
    GLsizei /*unused*/, const char* message, const void* /*unused*/
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

static void R_InitCubePrim(INOUT GfxCubePrim& cubePrim);
static void R_DrawFrameInternal();

constexpr float NEAR_PLANE_DEFAULT = 0.1f;
constexpr float FAR_PLANE_DEFAULT  = 100.0f;

GfxCubePrim r_cubePrim;
extern GfxFont r_defaultFont;
glm::mat4 r_orthoProjection;

float r_nearPlane, r_farPlane;

size_t r_testDrawId = 0;

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
    GL_CALL(glDebugMessageControl,  GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, (GLboolean)GL_TRUE);
#endif // _DEBUG

    GL_CALL(glClearColor, 0.2f, 0.3f, 0.3f, 1.0f);
    GL_CALL(glViewport,   0, 0, vid_width, vid_height);
    r_nearPlane = NEAR_PLANE_DEFAULT;
    r_farPlane  = FAR_PLANE_DEFAULT;

    r_orthoProjection = glm::ortho(0.0f, (float)vid_width, 0.0f, (float)vid_height);
    CG_Camera().perspectiveProjection = glm::perspective(FOV_HORZ_TO_VERTICAL(CG_Fov(), VID_ASPECT_INV()), VID_ASPECT(), r_nearPlane, r_farPlane);

    assert(R_CreateFont("consola.ttf", 0, 48, r_defaultFont));
    
    R_ClearTextDraws();
    R_AddTextDraw(nullptr, "This is a test.\nCan we wrap?", 0.1f, 0.1f, 1.0f, 1.0f, glm::vec3(0.77, 0.77, 0.2), true, false, r_testDrawId);
    R_InitCubePrim(r_cubePrim);
     
    //glEnable(GL_POINT_SMOOTH);
    //glPointSize(4);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
}

static void R_InitCubePrim(INOUT GfxCubePrim& cubePrim) {
    std::string vertSource = DB_LoadShader("vs.glsl");
    Com_DPrintln(vertSource);
    std::string fragSource = DB_LoadShader("fs.glsl");
    Com_DPrintln(fragSource);

    std::string errorLog;
    if (!R_CreateShaderProgram(vertSource, fragSource, &errorLog, r_cubePrim.prog))
        Com_Errorln(errorLog);

    glGenVertexArrays(1, &cubePrim.vao);
    glBindVertexArray(cubePrim.vao);

    glGenBuffers(1, &cubePrim.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, cubePrim.vbo);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(GfxCubePrim::vertices), 
        GfxCubePrim::vertices, GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(*GfxCubePrim::vertices), (void*)0
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
    R_SetUniform(cubePrim.prog.program, "uContainerTex", 0);
}

void R_DrawFrame() {
    RB_BeginFrame();

    R_DrawFrameInternal();

    RB_EndFrame();
}

void R_WindowResized() {
    r_orthoProjection = glm::ortho(0.0f, (float)vid_width, 0.0f, (float)vid_height);
    CG_Camera().perspectiveProjection = glm::perspective(FOV_HORZ_TO_VERTICAL(CG_Fov(), VID_ASPECT_INV()), VID_ASPECT(), r_nearPlane, r_farPlane);
    GL_CALL(glViewport, 0, 0, vid_width, vid_height); 
}

NO_DISCARD bool R_CreateImage(
    std::string_view image_name, OPTIONAL_OUT int* width, 
    OPTIONAL_OUT int* height, OUT texture_t& tex
) {
    if(width)
        *width  = 0;
    if(height)
        *height = 0;

    GL_CALL(glActiveTexture, GL_TEXTURE0);
    GL_CALL(glGenTextures,   1, &tex);
    GL_CALL(glBindTexture,   GL_TEXTURE_2D, tex);

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

void R_DrawCube(const glm::vec3& pos, float angle, texture_t tex) {
    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glBindVertexArray(r_cubePrim.vao);
    glBindBuffer(GL_ARRAY_BUFFER, r_cubePrim.vbo);
    glUseProgram(r_cubePrim.prog.program);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0, 0.3, 0.5));
    R_SetUniform(r_cubePrim.prog.program, "uModel", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_DEPTH_TEST);
}

static void R_DrawFrameInternal() {
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

    GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*
    R_DrawText(
        NULL, "Testing 123...", 
        25.0, 25.0, 1.0, 1.0, 
        glm::vec3(0.5, 0.8f, 0.2f)
    );
    */

    GL_CALL(glUseProgram, r_cubePrim.prog.program);
    glm::vec3 pos = CG_Camera().pos;
    glm::vec3 center = pos + CG_Camera().front;
    glm::vec3 up = CG_Camera().up;
    glm::mat4 view = glm::lookAt(pos, center, up);
    R_SetUniform(r_cubePrim.prog.program, "uView", view);

    for (int i = 0; i < (int)cubePositions.size(); i++) {
        R_DrawCube(cubePositions[i], 20.0f * (float)i, r_cubePrim.tex);
    }
    GL_CALL(glUseProgram, 0);

    R_DrawTextDraws();
}

void R_Shutdown() {
    IMG_Quit();
    RB_Shutdown();
}
