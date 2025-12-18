#include "shader.hpp"

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path) {
    std::string vs_code, fs_code, gs_code;
    bool geometry_defined = geometry_path.length() > 0;
    try {
        std::ifstream vs_file;
        vs_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        std::stringstream vs_stream;
        vs_file.open(vertex_path.c_str());
        vs_stream << vs_file.rdbuf();
        vs_file.close();
        vs_code = vs_stream.str();
    }
    catch (std::ifstream::failure e) {
        throw std::runtime_error("ERROR READING SHADER FILE: " + vertex_path + '\n' + e.what());
    }

    try {
        std::ifstream fs_file;
        fs_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        std::stringstream fs_stream;
        fs_file.open(fragment_path.c_str());
        fs_stream << fs_file.rdbuf();
        fs_file.close();
        fs_code = fs_stream.str();
    }
    catch (std::ifstream::failure e) {
        throw std::runtime_error("ERROR READING SHADER FILE: " + fragment_path + '\n' + e.what());
    }

    if (geometry_defined) {
        try {
            std::ifstream gs_file;
            gs_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            std::stringstream gs_stream;
            gs_file.open(geometry_path.c_str());
            gs_stream << gs_file.rdbuf();
            gs_file.close();
            fs_code = gs_stream.str();
        }
        catch (std::ifstream::failure e) {
            throw std::runtime_error("ERROR READING SHADER FILE: " + geometry_path + '\n' + e.what());
        }
    }

    const char* vs_cstr = vs_code.c_str();
    const char* fs_cstr = fs_code.c_str();
    const char* gs_cstr = gs_code.c_str();

    unsigned int vertex, fragment, geometry;
    int success;
    char info_log[512];


    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vs_cstr, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, info_log);
        throw std::runtime_error("ERROR VERTEX SHADER COMPILATION:\n" + std::string(info_log));
    }


    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fs_cstr, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, info_log);
        throw std::runtime_error("ERROR FRAGMENT SHADER COMPILATION\n" + std::string(info_log));
    }

    if (geometry_defined) {
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gs_cstr, NULL);
        glCompileShader(geometry);

        glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(geometry, 512, NULL, info_log);
            throw std::runtime_error("ERROR GEOMETRY SHADER COMPILATION\n" + std::string(info_log));
        }
    }

    program_id = glCreateProgram();
    glAttachShader(program_id, vertex);
    glAttachShader(program_id, fragment);
    if (geometry_defined) {
        glAttachShader(program_id, geometry);
    }
    glLinkProgram(program_id);

    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program_id, 512, NULL, info_log);
        throw std::runtime_error("ERROR PROGRAM COMPILATION\n" + std::string(info_log));
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometry_defined) {
        glDeleteShader(geometry);
    }
}

Shader& Shader::use() {
    glUseProgram(program_id);
    return *this;
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(program_id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(program_id, name.c_str()), (int)value);

}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(program_id, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(program_id, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(program_id, name.c_str()), x, y, z);
}

void Shader::setVec3(const std::string& name, const glm::vec3& v) const {
    Shader::setVec3(name, v.x, v.y, v.z);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(program_id, name.c_str()), x, y, z, w);
}

void Shader::setVec4(const std::string& name, const glm::vec4& v) const {
    Shader::setVec4(name, v.x, v.y, v.z, v.w);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(program_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}