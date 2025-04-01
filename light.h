#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <cmath>
#include <unordered_map>


// Light class
class Light {
protected:
    glm::vec3 position;
    glm::vec3 color;
    float intensity;

public:
    Light(glm::vec3 pos, glm::vec3 col, float inten)
        : position(pos), color(col), intensity(inten) {
    }

    virtual void UpdateLightProperties(GLuint shaderProgram) = 0;

    void SetPosition(glm::vec3 pos) { position = pos; }
    void SetColor(glm::vec3 col) { color = col; }
    void SetIntensity(float inten) { intensity = inten; }

    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetColor() const { return color; }
    float GetIntensity() const { return intensity; }
};

// Directional light class
class DirectionalLight : public Light {
private:
    glm::vec3 direction;

public:
    DirectionalLight(glm::vec3 pos, glm::vec3 target, glm::vec3 col, float inten)
        : Light(pos, col, inten) {
        SetDirection(target - pos);
    }

    // Function to update light properties
    void UpdateLightProperties(GLuint shaderProgram) override {
        // Pass the directional light's direction to the shader
        GLuint lightDirAddress = glGetUniformLocation(shaderProgram, "dirLightDirection");
        glUniform3fv(lightDirAddress, 1, glm::value_ptr(direction));

        // Pass the directional light's color to the shader
        GLuint lightColorAddress = glGetUniformLocation(shaderProgram, "dirLightColor");
        glUniform3fv(lightColorAddress, 1, glm::value_ptr(color));

        // Pass the directional light's brightness to the shader
        GLuint lightBrightnessAddress = glGetUniformLocation(shaderProgram, "dirLightBrightness");
        glUniform1f(lightBrightnessAddress, intensity);
    }

    void SetDirection(glm::vec3 dir) { direction = glm::normalize(dir); }
    glm::vec3 GetDirection() const { return direction; }
};

// Point light class
class PointLight : public Light {
public:
    PointLight(glm::vec3 pos, glm::vec3 col, float inten)
        : Light(pos, col, inten) {
    }

    // Function to update light properties
    void UpdateLightProperties(GLuint shaderProgram) override {
        // Pass the point light's position to the shader
        GLuint lightPosAddress = glGetUniformLocation(shaderProgram, "pointLightPos");
        glUniform3fv(lightPosAddress, 1, glm::value_ptr(position));

        // Pass the point light's color to the shader
        GLuint lightColorAddress = glGetUniformLocation(shaderProgram, "pointLightColor");
        glUniform3fv(lightColorAddress, 1, glm::value_ptr(color));

        // Pass the point light's brightness to the shader
        GLuint lightBrightnessAddress = glGetUniformLocation(shaderProgram, "pointLightBrightness");
        glUniform1f(lightBrightnessAddress, intensity);
    }
};
