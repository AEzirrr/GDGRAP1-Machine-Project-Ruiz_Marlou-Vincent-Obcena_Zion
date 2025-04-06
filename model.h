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

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

glm::mat4 identity_matrix(1.0);

class Model3D {
public:
    glm::vec3 position; // Position of the model
    glm::vec3 rotation; // Rotation in degrees
    glm::vec3 scale; // Scale of the model

    std::vector<GLfloat> fullVertexData; // Vertex data for the model
    GLuint VAO, VBO, texture, normalMap;
    bool hasTexture; // bool to check if the model has a texture
    bool hasNormalMap; // bool to check if the model has a normal map

    // Constructor with optional texture path and normal map path
    Model3D(const std::string& modelPath,
        const std::string& texturePath = "",
        const std::string& normalPath = "",
        glm::vec3 pos = glm::vec3(0.0f),
        glm::vec3 rot = glm::vec3(0.0f),
        glm::vec3 scl = glm::vec3(1.0f))
        : position(pos), rotation(rot), scale(scl),
        hasTexture(!texturePath.empty()),
        hasNormalMap(!normalPath.empty()) {
        loadModel(modelPath);
        if (hasTexture) {
            loadTexture(texturePath);
        }
        if (hasNormalMap) {
            loadNormal(normalPath);
        }
        setupBuffers();
    }

    // Function for loading a texture from an image file
    void loadTexture(const std::string& path) {
        stbi_set_flip_vertically_on_load(true);
        int img_width, img_height, color_channels;
        unsigned char* tex_bytes = stbi_load(path.c_str(), &img_width, &img_height, &color_channels, 0);

        if (!tex_bytes) {
            std::cerr << "Failed to load texture: " << path << std::endl;
            return;
        }

        // Determine the format based on the file extension
        GLenum format = GL_RGB;
        if (path.substr(path.find_last_of(".") + 1) == "png") {
            format = GL_RGBA;
        }

        // Create and bind the texture
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Load the texture data
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            img_width,
            img_height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            tex_bytes
        );

        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(tex_bytes); // Free image data
    }


    // Function for loading a normal map texture
    void loadNormal(const std::string& path) {
        stbi_set_flip_vertically_on_load(true);
        int img_width, img_height, color_channels;
        unsigned char* tex_bytes = stbi_load(path.c_str(), &img_width, &img_height, &color_channels, 0);

        if (!tex_bytes) {
            std::cerr << "Failed to load normal map: " << path << std::endl;
            return;
        }

        // Create and bind the normal map texture
        glGenTextures(1, &normalMap);
        glActiveTexture(GL_TEXTURE1); // Use texture unit 1 for normal map
        glBindTexture(GL_TEXTURE_2D, normalMap);

        // Load the texture data
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            img_width,
            img_height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            tex_bytes
        );

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(tex_bytes); // Free image data
    }

    // Modified loadModel function to handle normal mapping
    void loadModel(const std::string& path) {
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> material;
        std::string warning, error;

        tinyobj::attrib_t attributes;

        // Load the obj file
        bool success = tinyobj::LoadObj(
            &attributes,
            &shapes,
            &material,
            &warning,
            &error,
            path.c_str()
        );

        if (!success) {
            std::cerr << "Error loading model: " << error << std::endl;
            return;
        }

        // Calculate tangents and bitangents if we have a normal map
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;

        if (hasNormalMap) {
            for (int i = 0; i < shapes[0].mesh.indices.size(); i += 3) {
                tinyobj::index_t vData1 = shapes[0].mesh.indices[i];
                tinyobj::index_t vData2 = shapes[0].mesh.indices[i + 1];
                tinyobj::index_t vData3 = shapes[0].mesh.indices[i + 2];

                glm::vec3 v1 = glm::vec3(
                    attributes.vertices[vData1.vertex_index * 3],
                    attributes.vertices[vData1.vertex_index * 3 + 1],
                    attributes.vertices[vData1.vertex_index * 3 + 2]
                );

                glm::vec3 v2 = glm::vec3(
                    attributes.vertices[vData2.vertex_index * 3],
                    attributes.vertices[vData2.vertex_index * 3 + 1],
                    attributes.vertices[vData2.vertex_index * 3 + 2]
                );

                glm::vec3 v3 = glm::vec3(
                    attributes.vertices[vData3.vertex_index * 3],
                    attributes.vertices[vData3.vertex_index * 3 + 1],
                    attributes.vertices[vData3.vertex_index * 3 + 2]
                );

                glm::vec2 uv1 = glm::vec2(
                    attributes.texcoords[vData1.texcoord_index * 2],
                    attributes.texcoords[vData1.texcoord_index * 2 + 1]
                );

                glm::vec2 uv2 = glm::vec2(
                    attributes.texcoords[vData2.texcoord_index * 2],
                    attributes.texcoords[vData2.texcoord_index * 2 + 1]
                );

                glm::vec2 uv3 = glm::vec2(
                    attributes.texcoords[vData3.texcoord_index * 2],
                    attributes.texcoords[vData3.texcoord_index * 2 + 1]
                );

                glm::vec3 deltaPos1 = v2 - v1;
                glm::vec3 deltaPos2 = v3 - v1;

                glm::vec2 deltaUV1 = uv2 - uv1;
                glm::vec2 deltaUV2 = uv3 - uv1;

                float r = 1.0f / ((deltaUV1.x * deltaUV2.y) - (deltaUV1.y * deltaUV2.x));

                glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
                glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

                // Push the same tangent/bitangent for all three vertices of the triangle
                tangents.push_back(tangent);
                tangents.push_back(tangent);
                tangents.push_back(tangent);

                bitangents.push_back(bitangent);
                bitangents.push_back(bitangent);
                bitangents.push_back(bitangent);
            }
        }

        // Fill vertex data
        for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
            tinyobj::index_t vData = shapes[0].mesh.indices[i];

            // Position
            fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3)]);
            fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3) + 1]);
            fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3) + 2]);

            // Normal
            fullVertexData.push_back(attributes.normals[(vData.normal_index * 3)]);
            fullVertexData.push_back(attributes.normals[(vData.normal_index * 3) + 1]);
            fullVertexData.push_back(attributes.normals[(vData.normal_index * 3) + 2]);

            // Texture coordinates (if has texture or normal map)
            if (hasTexture || hasNormalMap) {
                fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2)]);
                fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2) + 1]);
            }

            // Tangent and bitangent (if has normal map)
            if (hasNormalMap) {
                fullVertexData.push_back(tangents[i].x);
                fullVertexData.push_back(tangents[i].y);
                fullVertexData.push_back(tangents[i].z);

                fullVertexData.push_back(bitangents[i].x);
                fullVertexData.push_back(bitangents[i].y);
                fullVertexData.push_back(bitangents[i].z);
            }
        }
    }

    // Modified setupBuffers function to handle normal mapping
    void setupBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER,
            sizeof(GLfloat) * fullVertexData.size(),
            fullVertexData.data(),
            GL_STATIC_DRAW);

        // Calculate stride based on what data we have
        GLsizei stride = 6 * sizeof(float); // position + normal

        if (hasTexture || hasNormalMap) {
            stride += 2 * sizeof(float); // add texture coordinates
        }
        if (hasNormalMap) {
            stride += 6 * sizeof(float); // add tangent + bitangent
        }

        // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(0);

        // Normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Texture coordinates (if present)
        if (hasTexture || hasNormalMap) {
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);
        }

        // Tangent and bitangent (if normal map present)
        if (hasNormalMap) {
            // Tangent (location = 3)
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
            glEnableVertexAttribArray(3);

            // Bitangent (location = 4)
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)(11 * sizeof(float)));
            glEnableVertexAttribArray(4);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // Function for computing the transformation matrix of the model
    glm::mat4 getTransform() {
        glm::mat4 transform = glm::translate(identity_matrix, glm::vec3(position.x, position.y, position.z));
        transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        transform = glm::scale(transform, scale);
        return transform;
    }

    void updateTransform(float rotx, float roty, float rotz, float xmod, float ymod, float zmod, float scaleVal) {
        rotation.x = rotx;
        rotation.y = roty;
        rotation.z = rotz;

        position.x = xmod;
        position.y = ymod;
        position.z = zmod;

        scale = glm::vec3(scaleVal);
    }

    // Draw function for rendering model
    void draw(GLuint shaderProg) {
        glBindVertexArray(VAO);

        // Bind diffuse texture if present
        if (hasTexture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glUniform1i(glGetUniformLocation(shaderProg, "tex0"), 0);
        }

        // Bind normal map if present
        if (hasNormalMap) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, normalMap);
            glUniform1i(glGetUniformLocation(shaderProg, "norm_tex"), 1);
        }

        // Set transform uniform
        unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(getTransform()));

        // Calculate vertex count
        int elementsPerVertex = 6; // position + normal
        if (hasTexture || hasNormalMap) elementsPerVertex += 2;
        if (hasNormalMap) elementsPerVertex += 6;

        GLsizei vertexCount = fullVertexData.size() / elementsPerVertex;
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }
};