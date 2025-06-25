#include "TextureLoader.h"

std::map<std::string, std::pair<std::string, std::shared_ptr<TextureKTX2>>> TextureLoader::textureKTX2Map;

std::shared_ptr<TextureKTX2> TextureLoader::AddKTX2Texture(std::string path, std::string texture_name)
{
	auto it = textureKTX2Map.find(texture_name);
	if (it == textureKTX2Map.end())
	{
		textureKTX2Map[texture_name] = std::make_pair(path, std::make_shared<TextureKTX2>(path));
	}
	return textureKTX2Map[texture_name].second;
}

std::shared_ptr<TextureKTX2> TextureLoader::GetKTX2Texture(std::string texture_name)
{
	auto it = textureKTX2Map.find(texture_name);
	if (it == textureKTX2Map.end())
	{
		std::cout << "The texture [Name: " << texture_name << "] required is not added!" << std::endl;
	}
	return textureKTX2Map[texture_name].second;
}
