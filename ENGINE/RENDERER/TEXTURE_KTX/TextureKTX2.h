#pragma once

#include <string>
#include <filesystem>
#include <iostream>
#include <array>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ktx.h>

class TextureKTX2
{
public:
	TextureKTX2();
	TextureKTX2(GLenum target);
	~TextureKTX2();

	void Bind();
	void UnBind();

	bool LoadTX2Texture(const char* filePath);
	bool LoadTX2Texture2D(const char* filePath);
	bool ConvertPNGtoKTX2Texture(const std::string& pngSourcePath, const std::string& ktxOutputPath);
private:
	GLuint textureID;
	GLenum target;
};