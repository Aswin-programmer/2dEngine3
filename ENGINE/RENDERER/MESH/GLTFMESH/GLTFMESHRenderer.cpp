#include "GLTFMESHRenderer.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION

#define BUFFER_OFFSET(i) ((void*)(size_t)(i))

GLTFMESHRenderer::GLTFMESHRenderer()
{
    SetupGLTFMESHRenderer();
}

GLTFMESHRenderer::GLTFMESHRenderer(std::string filePath)
    : filePath{ filePath }
{
    SetupGLTFMESHRenderer();
}

GLTFMESHRenderer::~GLTFMESHRenderer()
{
    for (auto& p : primitives)
    {
        if (p.vao) glDeleteVertexArrays(1, &p.vao);
        if (p.ebo) glDeleteBuffers(1, &p.ebo);
        if (p.posVBO) glDeleteBuffers(1, &p.posVBO);
        if (p.normVBO) glDeleteBuffers(1, &p.normVBO);
        if (p.texVBO) glDeleteBuffers(1, &p.texVBO);
    }
}

static GLenum ComponentTypeToGL(int componentType)
{
    switch (componentType)
    {
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:  return GL_UNSIGNED_BYTE;
    case TINYGLTF_COMPONENT_TYPE_BYTE:           return GL_BYTE;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: return GL_UNSIGNED_SHORT;
    case TINYGLTF_COMPONENT_TYPE_SHORT:          return GL_SHORT;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:   return GL_UNSIGNED_INT;
    case TINYGLTF_COMPONENT_TYPE_INT:            return GL_INT;
    case TINYGLTF_COMPONENT_TYPE_FLOAT:          return GL_FLOAT;
    case TINYGLTF_COMPONENT_TYPE_DOUBLE:         return GL_DOUBLE;
    default: return GL_UNSIGNED_SHORT;
    }
}

void GLTFMESHRenderer::SetupGLTFMESHRenderer()
{
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, filePath);

    if (!warn.empty()) std::cout << "WARN: " << warn << std::endl;
    if (!err.empty()) std::cout << "ERR: " << err << std::endl;
    if (!ret) {
        std::cout << "Failed to load glTF: " << filePath << std::endl;
        return;
    }

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    for (int nodeIdx : scene.nodes)
    {
        ProcessNode(nodeIdx, glm::mat4(1.0f));
    }
}

void GLTFMESHRenderer::ProcessNode(int nodeIndex, const glm::mat4& parentTransform)
{
    const tinygltf::Node& node = model.nodes[nodeIndex];

    // Build local transform
    glm::mat4 localTransform(1.0f);

    if (node.matrix.size() == 16) {
        // tinygltf stores as double, convert to float
        glm::mat4 temp;
        for (int i = 0; i < 16; ++i) {
            reinterpret_cast<float*>(&temp)[i] = static_cast<float>(node.matrix[i]);
        }
        localTransform = temp;
    }
    else {
        if (node.translation.size() == 3) {
            localTransform = glm::translate(localTransform,
                glm::vec3(
                    static_cast<float>(node.translation[0]),
                    static_cast<float>(node.translation[1]),
                    static_cast<float>(node.translation[2])
                )
            );
        }
        if (node.rotation.size() == 4) {
            glm::quat q(
                static_cast<float>(node.rotation[3]), // w
                static_cast<float>(node.rotation[0]),
                static_cast<float>(node.rotation[1]),
                static_cast<float>(node.rotation[2])
            );
            localTransform *= glm::mat4_cast(q);
        }
        if (node.scale.size() == 3) {
            localTransform = glm::scale(localTransform,
                glm::vec3(
                    static_cast<float>(node.scale[0]),
                    static_cast<float>(node.scale[1]),
                    static_cast<float>(node.scale[2])
                )
            );
        }
    }

    glm::mat4 globalTransform = parentTransform * localTransform;

    // If node has a mesh, process its primitives
    if (node.mesh >= 0) {
        const tinygltf::Mesh& mesh = model.meshes[node.mesh];
        for (const auto& primitive : mesh.primitives)
        {
            PrimitiveGL p;
            p.modelMatrix = globalTransform; // store transform

            glGenVertexArrays(1, &p.vao);
            glBindVertexArray(p.vao);

            // --- Indices (EBO) ---
            if (primitive.indices >= 0) {
                const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
                const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
                const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];

                glGenBuffers(1, &p.ebo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p.ebo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    indexBufferView.byteLength,
                    indexBuffer.data.data() + indexBufferView.byteOffset,
                    GL_STATIC_DRAW);

                p.indexCount = indexAccessor.count;
                p.indexType = ComponentTypeToGL(indexAccessor.componentType);
            }

            // --- Vertex Attributes ---
            for (const auto& attrib : primitive.attributes)
            {
                const std::string attrName = attrib.first;
                const tinygltf::Accessor& accessor = model.accessors[attrib.second];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                int location = -1;
                GLuint* targetVBO = nullptr;

                if (attrName == "POSITION") {
                    location = 0;
                    glGenBuffers(1, &p.posVBO);
                    targetVBO = &p.posVBO;
                }
                else if (attrName == "NORMAL") {
                    location = 1;
                    glGenBuffers(1, &p.normVBO);
                    targetVBO = &p.normVBO;
                }
                else if (attrName == "TEXCOORD_0") {
                    location = 2;
                    glGenBuffers(1, &p.texVBO);
                    targetVBO = &p.texVBO;
                }

                if (location >= 0 && targetVBO) {
                    glBindBuffer(GL_ARRAY_BUFFER, *targetVBO);
                    glBufferData(GL_ARRAY_BUFFER,
                        bufferView.byteLength,
                        buffer.data.data() + bufferView.byteOffset,
                        GL_STATIC_DRAW);

                    int numComponents = accessor.type == TINYGLTF_TYPE_SCALAR ? 1 : accessor.type;
                    GLenum compType = ComponentTypeToGL(accessor.componentType);
                    int byteStride = accessor.ByteStride(bufferView);
                    if (byteStride == 0) {
                        int componentSize = 1;
                        switch (accessor.componentType) {
                        case TINYGLTF_COMPONENT_TYPE_FLOAT: componentSize = 4; break;
                        case TINYGLTF_COMPONENT_TYPE_DOUBLE: componentSize = 8; break;
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                        case TINYGLTF_COMPONENT_TYPE_INT: componentSize = 4; break;
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                        case TINYGLTF_COMPONENT_TYPE_SHORT: componentSize = 2; break;
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                        case TINYGLTF_COMPONENT_TYPE_BYTE: componentSize = 1; break;
                        }
                        byteStride = numComponents * componentSize;
                    }

                    glEnableVertexAttribArray(location);
                    glVertexAttribPointer(location, numComponents, compType,
                        accessor.normalized ? GL_TRUE : GL_FALSE,
                        byteStride, BUFFER_OFFSET(accessor.byteOffset));
                }
            }

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            primitives.push_back(p);
        }
    }

    // Recursively process children
    for (int childIndex : node.children) {
        ProcessNode(childIndex, globalTransform);
    }
}

void GLTFMESHRenderer::GLTFMESHDraw(Shader& shader)
{
    shader.use();

    for (const auto& p : primitives)
    {
        shader.setMat4("uModel", p.modelMatrix);
        glBindVertexArray(p.vao);
        glDrawElements(GL_TRIANGLES, p.indexCount, p.indexType, (void*)0);
    }

    glBindVertexArray(0);
    shader.unuse();
}
