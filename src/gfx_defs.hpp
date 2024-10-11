#pragma once

#include <memory>
#include <span>

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include "acommon/a_math.h"

#include "com_print.hpp"
#include "dvar.hpp"

#define R_SURF_MAX_VERTS 8
#define R_SURF_MAX_TRIS (R_SURF_MAX_VERTS - 2)

#define R_SURF_MIN_VERTS 3 // can't have a polygon with less than 3 vertices
#define R_SURF_MIN_TRIS  1 

typedef unsigned int vbo_t;
typedef unsigned int vao_t;
typedef unsigned int ebo_t;
typedef unsigned int vertex_shader_t;
typedef unsigned int fragment_shader_t;
typedef unsigned int shader_program_t;
typedef unsigned int texture_t;
typedef unsigned int image_format_t;

constexpr inline float VFOV_DEFAULT = 74.0f;

extern dvar_t* vid_width;
extern dvar_t* vid_height;

struct GfxSubTexDef {
    float x, y, u, v;
};

struct GfxCamera {
	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 worldUp;
	glm::vec3 right;
	glm::vec3 up;
	float     pitch;
	float     yaw;
    glm::mat4 perspectiveProjection;
    glm::mat4 orthoProjection;
};

struct GfxShaderProgram {
	shader_program_t  program;
	vertex_shader_t   vertex_shader;
	fragment_shader_t fragment_shader;
    
    GfxShaderProgram() {
        program = 0;
        vertex_shader = 0;
        fragment_shader = 0;
    }

    GfxShaderProgram(const GfxShaderProgram&) = delete;
    GfxShaderProgram(GfxShaderProgram&& other) noexcept {
        program = other.program;
        vertex_shader = other.vertex_shader;
        fragment_shader = other.fragment_shader;
        other.program = 0;
        other.vertex_shader = 0;
        other.fragment_shader = 0;
    }

    GfxShaderProgram& operator=(const GfxShaderProgram&) = delete;
    GfxShaderProgram& operator=(GfxShaderProgram&& other) noexcept {
        program = other.program;
        vertex_shader = other.vertex_shader;
        fragment_shader = other.fragment_shader;

        other.program = 0;
        other.vertex_shader = 0;
        other.fragment_shader = 0;
        return *this;
    }
 
    ~GfxShaderProgram() {
        glDeleteShader(fragment_shader);
        glDeleteShader(vertex_shader);
        glDeleteProgram(program);
    }
};

struct GfxCubePrim {
	inline static const float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    GfxShaderProgram prog;
    texture_t tex;
    vbo_t vbo;
    vao_t vao;
};

struct SdlSurfaceDeleter {
	void inline operator()(SDL_Surface* p) {
		SDL_DestroySurface(p);
	}
};

using SdlSurface = std::unique_ptr<SDL_Surface, SdlSurfaceDeleter>;

struct FontDef;

struct GfxTextDraw {
    bool free;
    FontDef* font;
    std::string text;
    RectDef rect;
    float xscale, yscale;
    glm::vec3 color;
    bool active;
    bool right;
};

A_NO_DISCARD constexpr inline std::string_view GL_ERROR_STR(GLenum err) {
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

A_NO_DISCARD constexpr inline std::string_view GL_DEBUG_SOURCE_STR(GLenum source) {
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

A_NO_DISCARD constexpr inline std::string_view GL_DEBUG_TYPE_STR(GLenum type) {
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

A_NO_DISCARD constexpr inline std::string_view GL_DEBUG_SEVERITY_STR(GLenum severity) {
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

inline GLenum R_GlCheckError(int line, const char* file) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::string_view s = GL_ERROR_STR(err);
        Com_Println(
            CON_DEST_ERR,
            "GL call at line {} in {} failed with {}.", line, file, s
        );
    }
    return err;
}

template<typename Func, typename...Args>
auto inline R_GlCallImpl(
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
auto inline R_GlCallImpl(
    int line, const char* file, Func func, Args... args
) -> typename std::enable_if_t<
    std::is_same_v<void, decltype(func(args...))>,
    GLenum
> {
    func(std::forward<Args>(args)...);
    return R_GlCheckError(line, file);
}

#ifndef GL_CALL
#define GL_CALL(func, ...)                                           \
    R_GlCallImpl(__LINE__, __FILE__, func __VA_OPT__(,) __VA_ARGS__);
#endif // GL_CALL

// can't be constexpr since atan() isn't constexpr for some reason
A_NO_DISCARD inline float FOV_HORZ_TO_VERTICAL(float fovx, float aspect_inv) {
    return 2.0f * glm::degrees(A_atanf(A_tanf(glm::radians(fovx) / 2) * aspect_inv));
}

A_NO_DISCARD inline float FOV_VERTICAL_TO_HORZ(float fovy, float aspect) {
    return 2.0f * glm::degrees(A_atanf(A_tanf(glm::radians(fovy) / 2) * aspect));
}

A_NO_DISCARD inline float VID_ASPECT() {
    return (float)Dvar_GetInt(*vid_width) / (float)Dvar_GetInt(*vid_width);
}

A_NO_DISCARD inline float VID_ASPECT_INV() {
    return (float)Dvar_GetInt(*vid_width) / (float)Dvar_GetInt(*vid_width);
}
