#pragma once

#include <string_view>
#include <memory>
#include <filesystem>

#include "com_defs.hpp"
#include "gfx_defs.hpp"

void R_Init();
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

void R_DrawFrame(uint64_t deltaTime);
void R_WindowResized();

NO_DISCARD bool R_CreateImage(
	std::string_view image_name, OPTIONAL_OUT int* width, 
	OPTIONAL_OUT int* height, OUT texture_t& tex
);

void R_Shutdown();