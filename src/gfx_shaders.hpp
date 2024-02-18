#pragma once

#include <string>

#include "com_defs.hpp"
#include "gfx_defs.hpp"

NO_DISCARD bool R_CompileShader(
    const std::string& shaderSource, int type,
    OPTIONAL_OUT std::string* log, OUT unsigned int& shader
);
NO_DISCARD bool R_LinkShaders(
    vertex_shader_t vertShader, fragment_shader_t fragShader,
    OPTIONAL_OUT std::string* log, OUT shader_program_t& program
);
NO_DISCARD bool R_CreateShaderProgram(
    const std::string& vertexSource, const std::string& fragmentSource,
    OPTIONAL_OUT std::string* log, OUT GfxShaderProgram& prog
);