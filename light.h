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

#define MAX_POINT_LIGHTS 10

glm::vec3 warmLightColor = glm::vec3(1.0f, 0.7f, 0.5f);  // Orange-ish warm light
glm::vec3 coldLightColor = glm::vec3(0.7f, 0.8f, 1.0f);  // Blue-ish cold light
bool isWarmLight = true;  // Start with warm light

//Light position
glm::vec3 lightPos = glm::vec3(0, 0, 0);
glm::vec3 lightColor = warmLightColor;  // Start with warm light
float brightness = 1;
float pointLightBrightness = 0;
glm::vec3 ambientColor = warmLightColor * 0.5f;  // Match ambient
float ambientStr = 0.5f;
float specStr = 3.0f;
float specPhong = 20;

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


class LightManager {
public:
    std::vector<PointLight> pointLights;

    void AddPointLight(const PointLight& light) {
        pointLights.push_back(light);
    }

    void UploadLights(GLuint shaderProgram) {
        // Upload point light count
        glUniform1i(glGetUniformLocation(shaderProgram, "numPointLights"), pointLights.size());

        // Prepare arrays
        glm::vec3 positions[MAX_POINT_LIGHTS];
        glm::vec3 colors[MAX_POINT_LIGHTS];
        float intensities[MAX_POINT_LIGHTS];

        for (size_t i = 0; i < pointLights.size() && i < MAX_POINT_LIGHTS; ++i) {
            positions[i] = pointLights[i].GetPosition();
            colors[i] = pointLights[i].GetColor();
            intensities[i] = pointLights[i].GetIntensity();
        }

        // Upload arrays to the shader
        glUniform3fv(glGetUniformLocation(shaderProgram, "pointLightPos"), MAX_POINT_LIGHTS, glm::value_ptr(positions[0]));
        glUniform3fv(glGetUniformLocation(shaderProgram, "pointLightColor"), MAX_POINT_LIGHTS, glm::value_ptr(colors[0]));
        glUniform1fv(glGetUniformLocation(shaderProgram, "pointLightBrightness"), MAX_POINT_LIGHTS, intensities);
    }
};
