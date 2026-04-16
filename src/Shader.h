#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    // Reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    // Prevent copying to avoid double-freeing the OpenGL Program ID
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Activate the shader
    void use() const;

    // Utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

    void setVec4(const std::string& name, const glm::vec4& value) const;

    unsigned int ID;

private:
    // Utility function for checking shader compilation/linking errors.
    void checkCompileErrors(unsigned int shader, std::string type);
};