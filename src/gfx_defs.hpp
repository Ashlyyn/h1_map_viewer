#pragma once

#include <memory>

#include "GL/glew.h"
#include <glm/glm.hpp>
#include <SDL3/SDL.h>

#include "font.hpp"

typedef unsigned int vbo_t;
typedef unsigned int vao_t;
typedef unsigned int ebo_t;
typedef unsigned int vertex_shader_t;
typedef unsigned int fragment_shader_t;
typedef unsigned int shader_program_t;
typedef unsigned int texture_t;
typedef unsigned int image_format_t;

constexpr inline float VFOV_DEFAULT = 74.0f;

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
};

struct GfxShaderProgram {
	shader_program_t  program;
	vertex_shader_t   vertex_shader;
	fragment_shader_t fragment_shader;

    ~GfxShaderProgram() {
        glDeleteShader(fragment_shader);
        glDeleteShader(vertex_shader);
        glDeleteProgram(program);
    }
};

struct GfxGlyph {
    int width, height;
    int left;
    int top;
    int advance_x, advance_y;
    float atlas_x, atlas_y;
};

struct GfxFont {
    std::array<GfxGlyph, 95> glyphs;
    int atlas_width, atlas_height;
    GfxShaderProgram prog;
    vao_t vao;
    vbo_t vbo;
    texture_t tex;

    ~GfxFont() {
        glDeleteTextures(1, &tex);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &vao);
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