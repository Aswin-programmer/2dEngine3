#pragma once

#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../SHADERS/Shader.h"
#include "../../CAMERA/Camera.h"
#include "../../TEXTURE_KTX/TextureLoader.h"

constexpr int MAX_MESH_VERTEX_DATA = 1000;

struct MeshStructureForRendering
{
	std::string meshName;
	int meshSize;
	int meshInstances;
	int meshVertexBufferOffset;
	int meshIndexBufferOffset;
};

struct MeshOrientation
{
	glm::vec4 Position;
	glm::vec4 Rotation;
	glm::vec4 Scale;

	MeshOrientation(glm::vec3 positon, glm::vec3 rotation, glm::vec3 scale)
		:
		Position{glm::vec4(positon.x, positon.y, positon.z, 1.f)},
		Rotation{ glm::vec4(rotation.x, rotation.y, rotation.z, 1.f) },
		Scale{ glm::vec4(scale.x, scale.y, scale.z, 1.f) }
	{

	}
};


class BasicMeshRenderer
{
public:
	BasicMeshRenderer();
	~BasicMeshRenderer();

	void AddMesh(std::string meshName, MeshOrientation meshOrientation);

	void CleanUp();

	void Render();

private:
	GLuint BasicMeshVAO, BasicMeshVBO, BasicMeshEBO;

	void setupBuffers();

	bool IsBuffersUpdateRequired;
	int meshVerticesBufferOffset;
	int meshIndicesBufferOffset;
	std::vector<float> meshVertices;
	std::vector<unsigned int> meshIndices;
	std::unordered_map<std::string, MeshStructureForRendering> meshStructureForRendering;

	GLuint BasicMeshOrientationSSBO;
	std::unordered_map<std::string, std::vector<MeshOrientation>> meshesOrientation;
};