#pragma once

#include <memory>
#include <map>

#include "TextureKTX2.h"

class TextureLoader
{
public:
	static std::shared_ptr<TextureKTX2> AddKTX2Texture(std::string path, std::string texture_name);
	static std::shared_ptr<TextureKTX2> GetKTX2Texture(std::string texture_name);
private:
	static std::map<std::string, std::pair<std::string, std::shared_ptr<TextureKTX2>>> textureKTX2Map; // <name, <path, shared_ptr>>
};