#include "gfx_shaders.hpp"

NO_DISCARD bool R_CompileShader(
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

NO_DISCARD bool R_LinkShaders(
    vertex_shader_t vertShader, fragment_shader_t fragShader,
    OPTIONAL_OUT std::string* log, OUT shader_program_t& program
) {
    program = GL_CALL(glCreateProgram);
    GL_CALL(glAttachShader, program, vertShader);
    GL_CALL(glAttachShader, program, fragShader);
    GL_CALL(glLinkProgram, program);

    if (log)
        *log = std::string();
    int success = GL_FALSE;
    GL_CALL(glGetProgramiv, program, GL_LINK_STATUS, &success);
    if (!success && log) {
        GLint len = 0;
        GL_CALL(glGetProgramiv, program, GL_INFO_LOG_LENGTH, &len);
        log->resize(len);
        GL_CALL(glGetProgramInfoLog, program, len, nullptr, log->data());
    }

    return success == GL_TRUE;
}

NO_DISCARD bool R_CreateShaderProgram(
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