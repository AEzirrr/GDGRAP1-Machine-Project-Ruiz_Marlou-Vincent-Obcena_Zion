#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

class Shader {
public:
    GLuint ID; // Shader program ID

    // Constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath) {
        std::fstream vertSrc(vertexPath);
        std::stringstream vertBuff;
        vertBuff << vertSrc.rdbuf();
        std::string vertS = vertBuff.str();
        const char* v = vertS.c_str();

        // Load fragment shader
        std::fstream fragSrc(fragmentPath);
        std::stringstream fragBuff;
        fragBuff << fragSrc.rdbuf();
        std::string fragS = fragBuff.str();
        const char* f = fragS.c_str();

        GLuint vertex, fragment;

        // Vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &v, NULL);
        glCompileShader(vertex);

        // Fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &f, NULL);
        glCompileShader(fragment);

        // Shader program
        ID = glCreateProgram();
        glAttachShader(vertex, fragment);
        glAttachShader(fragment, vertex);
        glLinkProgram(ID);

        // Delete the shaders as they're linked into our program now and no longer necessary
        //glDeleteShader(vertex);
        //glDeleteShader(fragment);
    }

    // Use/activate the shader
    void use() {
        glUseProgram(ID);
    }

    // Utility uniform functions
    void setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setVec2(const std::string& name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setVec4(const std::string& name, const glm::vec4& value) const {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setMat2(const std::string& name, const glm::mat2& mat) const {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setMat3(const std::string& name, const glm::mat3& mat) const {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

};