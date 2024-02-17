#pragma once

#include <vector>
#include <string_view>

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gfx_defs.hpp"

void inline R_SetUniform(
    shader_program_t program, const std::string& name, float value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniform1f(location, value);
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, 
    const float* value, int count
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniform1fv(location, count, value);
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, 
    const std::vector<float>& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniform1fv(location, (GLsizei)value.size(), value.data());
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, const glm::vec2& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniform2f(location, value.x, value.y);
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, const glm::vec3& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniform3f(location, value.x, value.y, value.z);
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, const glm::vec4& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, int value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniform1i(location, value);
 }

void inline R_SetUniform(
    shader_program_t program, const std::string& name, 
    const int* value, int count
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniform1iv(location, count, value);
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, 
    const std::vector<int>& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniform1iv(location, (GLsizei)value.size(), value.data());
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, const glm::ivec2& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniform2i(location, value.x, value.y);
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, const glm::ivec3& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniform3i(location, value.x, value.y, value.z);
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, const glm::ivec4& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniform4i(location, value.x, value.y, value.z, value.w);
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, unsigned int value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniform1ui(location, value);
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, 
    const unsigned int* value, int count
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniform1uiv(location, count, value);
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, 
    const std::vector<unsigned int>& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniform1uiv(location, (GLsizei)value.size(), value.data());
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, const glm::uvec2& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniform2ui(location, value.x, value.y);
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, const glm::uvec3& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniform3ui(location, value.x, value.y, value.z);
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, const glm::uvec4& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniform4ui(location, value.x, value.y, value.z, value.w);
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, const glm::mat2& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, const glm::mat2x3& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniformMatrix2x3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, const glm::mat2x4& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniformMatrix2x4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, const glm::mat3x2& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniformMatrix3x2fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, const glm::mat3& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, const glm::mat3x4& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniformMatrix3x4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, const glm::mat4x2& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniformMatrix4x2fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, const glm::mat4x3& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniformMatrix4x3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void inline R_SetUniform(
    shader_program_t program, const std::string& name, const glm::mat4& value
) {
    int location = glGetUniformLocation(program, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

template<size_t N>
void inline R_SetUniform(
    shader_program_t program, const std::string& name,
    const std::array<float, N>& value
) {
    int location = glGetUniformLocation(program, name);
    glUniform1fv(location, N, value.data());
}

template<size_t N>
void inline R_SetUniform(
    shader_program_t program, const std::string& name, 
    const std::array<int, N>& value
) {
    int location = glGetUniformLocation(program, name);
    glUniform1iv(location, N, value.data());
}

template<size_t N>
void inline R_SetUniform(
    shader_program_t program, const std::string& name, 
    const std::array<unsigned int, N>& value
) {
    int location = glGetUniformLocation(program, name);
    glUniform1uiv(location, N, value.data());
}

/*
template<typename T>
concept uniform_data = requires(T) {
    std::same_as<T, float> || std::same_as<T, int> 
        || std::same_as<T, unsigned int>|| std::same_as<T, glm::vec2>   
        || std::same_as<T, glm::ivec2>  || std::same_as<T, glm::uvec2>
        || std::same_as<T, glm::vec3>   || std::same_as<T, glm::ivec3> 
        || std::same_as<T, glm::uvec3>  || std::same_as<T, glm::vec4>   
        || std::same_as<T, glm::ivec4>  || std::same_as<T, glm::uvec4>
        || std::same_as<T, glm::mat2>   || std::same_as<T, glm::mat2x3> 
        || std::same_as<T, glm::mat2x4> || std::same_as<T, glm::mat3x2> 
        || std::same_as<T, glm::mat3>   || std::same_as<T, glm::mat3x4>
        || std::same_as<T, glm::mat4x2> || std::same_as<T, glm::mat4x3> 
        || std::same_as<T, glm::mat4>;
};

template<uniform_data T>
class Uniform {
private:
    T m_data;
    std::string m_name;
    shader_program_t m_prog;

public:
    using self = Uniform<T>;
    using value_type = T;

    Uniform(
        shader_program_t program, std::string_view name, value_type value
     ) {
        m_prog = program;
        m_name = name;
        m_data = value;

        R_SetUniform(program, name, value);
    }

    Uniform(const self& other) = default;
    Uniform(self&& other) = default;

    self& operator=(const self& other) = default;

    self& operator=(const value_type& other) {
        m_data = other;
        R_SetUniform(m_prog, m_name, other);
        return this;
    }

    bool operator==(const self& rhs) const {
        return m_data == rhs.m_data;
    }

    bool operator==(const value_type& rhs) const {
        return m_data == rhs;
    }

    self operator+(const self& rhs) const {
        self s = *this;
        s.m_data += rhs.m_data;
        return s;
    }

    value_type operator+(const value_type& rhs) const {
        return m_data + rhs;
    }

    self operator-(const self& rhs) const {
        self s = *this;
        s.m_data -= rhs.m_data;
        return s;
    }

    value_type operator-(const value_type& rhs) const {
        return m_data - rhs;
    }

    self operator*(const self& rhs) const {
        self s = *this;
        s.m_data *= rhs.m_data;
        return s;
    }

    value_type operator*(const value_type& rhs) const {
        return m_data * rhs;
    }

    self operator/(const self& rhs) const {
        self s = *this;
        s.m_data /= rhs.m_data;
        return s;
    }

    value_type operator/(const value_type& rhs) const {
        return m_data / rhs;
    }

    const self& operator+=(const self& rhs) const {
        m_data += rhs.m_data;
        R_SetUniform(m_prog, m_name, m_data);
        return this;
    }

    self& operator+=(const self& rhs) {
        m_data += rhs.m_data;
        R_SetUniform(m_prog, m_name, m_data);
        return this;
    }

    const self& operator+=(const value_type& rhs) const {
        m_data += rhs;
        R_SetUniform(m_prog, m_name, m_data);
        return this;
    }

    self& operator+=(const value_type& rhs) {
        m_data += rhs;
        R_SetUniform(m_prog, m_name, m_data);
        return this;
    }

    const self& operator-=(const self& rhs) const {
        m_data -= rhs.m_data;
        R_SetUniform(m_prog, m_name, m_data);
        return this;
    }

    self& operator-=(const self& rhs) {
        m_data -= rhs.m_data;
        R_SetUniform(m_prog, m_name, m_data);
        return this;
    }

    const self& operator-=(const value_type& rhs) const {
        m_data -= rhs;
        R_SetUniform(m_prog, m_name, m_data);
        return this;
    }

    self& operator-=(const value_type& rhs) {
        m_data -= rhs;
        R_SetUniform(m_prog, m_name, m_data);
        return this;
    }

    const self& operator*=(const self& rhs) const {
        m_data *= rhs.m_data;
        R_SetUniform(m_prog, m_name, m_data);
        return this;
    }

    self& operator*=(const self& rhs) {
        m_data *= rhs.m_data;
        R_SetUniform(m_prog, m_name, m_data);
        return this;
    }

    const self& operator*=(const value_type& rhs) const {
        m_data *= rhs;
        R_SetUniform(m_prog, m_name, m_data);
        return this;
    }

    self& operator*=(const value_type& rhs) {
        m_data *= rhs;
        R_SetUniform(m_prog, m_name, m_data);
        return this;
    }

    const self& operator/=(const self& rhs) const {
        m_data /= rhs.m_data;
        R_SetUniform(m_prog, m_name, m_data);
        return this;
    }

    self& operator/=(const self& rhs) {
        m_data /= rhs.m_data;
        R_SetUniform(m_prog, m_name, m_data);
        return this;
    }

    const self& operator/=(const value_type& rhs) const {
        m_data /= rhs;
        R_SetUniform(m_prog, m_name, m_data);
        return this;
    }

    self& operator/=(const value_type& rhs) {
        m_data /= rhs;
        R_SetUniform(m_prog, m_name, m_data);
        return this;
    }

    operator value_type() const {
        return m_data;
    }

    operator value_type& () {
        return m_data;
    }

    operator const value_type& () const {
        return m_data;
    }
};
*/