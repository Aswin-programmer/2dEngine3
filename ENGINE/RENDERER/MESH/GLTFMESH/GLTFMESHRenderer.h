#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tiny_gltf.h>

#include "../../SHADERS/Shader.h"

class GLTFMESHRenderer
{
public:
    GLTFMESHRenderer();
    GLTFMESHRenderer(std::string fileName);
    ~GLTFMESHRenderer();

    void SetupGLTFMESHRenderer();
    void GLTFMESHDraw(Shader& shader);

private:
    std::string filePath;
    tinygltf::Model model;

    struct PrimitiveGL {
        GLuint vao = 0;
        GLuint ebo = 0;
        GLuint posVBO = 0;
        GLuint normVBO = 0;
        GLuint texVBO = 0;
        int indexCount = 0;
        GLenum indexType = GL_UNSIGNED_SHORT;
        glm::mat4 modelMatrix = glm::mat4(1.0f); // NEW: node transform
    };

    std::vector<PrimitiveGL> primitives;

    void ProcessNode(int nodeIndex, const glm::mat4& parentTransform);
};
