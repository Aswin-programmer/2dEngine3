#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <tiny_gltf.h>

class GLTFMESHLoader
{
public:
	GLTFMESHLoader(const GLTFMESHLoader&) = delete;
	GLTFMESHLoader& operator=(const GLTFMESHLoader) = delete;

	static bool LoadGLTFModel(std::string filePath);
	static tinygltf::Model& GetGLTFModel(std::string fileName);
	static void ClearAllGLTFModels();

private:
	static std::unordered_map<std::string, tinygltf::Model> GLTFModels;
};
