#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Perspective Cam Pos
float PerscCamX = 0;
float PerscCamY = 0;
float PerscCamZ = 0;

// Ortho Cam Pos
float orthoCamX = -30.0f;
float orthoCamY = 25.0f;
float orthoCamZ = -30.0f;

// Camera class
class MyCamera {
protected:
    glm::vec3 front; // Front dir
    glm::vec3 up; // Up dir
    float yaw; // Yaw angle
    float pitch; // Pitch angle
    float distance; // Distance from the target

public:
    glm::vec3 position;

    // Constructor
    MyCamera(glm::vec3 pos, glm::vec3 frontDir, glm::vec3 upDir, float y, float p, float dist)
        : position(pos), front(frontDir), up(upDir), yaw(y), pitch(p), distance(dist) {
        UpdateCameraVectors();
    }

    virtual glm::mat4 GetProjectionMatrix(float aspect) const = 0;

    // Function to get the view matrix
    glm::mat4 GetViewMatrix(const glm::vec3& target) const {
        return glm::lookAt(position, target, up);
    }

    // Function to process mouse movement
    void ProcessMouseMovement(float xoffset, float yoffset, float sensitivity = 0.1f) {
        yaw += xoffset * sensitivity;
        pitch += yoffset * sensitivity;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        UpdateCameraVectors();
    }

    // Function to update the camera position
    void UpdateCameraPosition(const glm::vec3& target) {
        position.x = target.x + distance * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        position.y = target.y + distance * sin(glm::radians(pitch));
        position.z = target.z + distance * sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    }

private:
    // Function to update the camera vectors
    void UpdateCameraVectors() {
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);
    }
};

// Perspective camera class
class PerspectiveCamera : public MyCamera {
private:
    float fov;

public:
    // Constructor
    PerspectiveCamera(glm::vec3 pos, glm::vec3 frontDir, glm::vec3 upDir, float y, float p, float dist, float fovAngle)
        : MyCamera(pos, frontDir, upDir, y, p, dist), fov(fovAngle) {
    }

    // Function to get the projection matrix
    glm::mat4 GetProjectionMatrix(float aspect) const override {
        return glm::perspective(glm::radians(fov), aspect, 0.1f, 100.0f);
    }
};

bool usePerspective = true;
PerspectiveCamera perspectiveCam(
    glm::vec3(0.0f, 0.0f, 10.0f), // Position
    glm::vec3(0.0f, 0.0f, -1.0f), // Front dir
    glm::vec3(0.0f, 1.0f, 0.0f), // Up dir
    270.0f, // Yaw
    0.0f, // Pitch
    15.0f, // Distance
    70.0f // FOV
);
