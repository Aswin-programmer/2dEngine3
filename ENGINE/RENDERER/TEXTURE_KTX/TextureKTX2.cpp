#include "TextureKTX2.h"

TextureKTX2::TextureKTX2()
	:
	textureID{0},
	target{GL_TEXTURE_2D}
{
}

TextureKTX2::~TextureKTX2()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &textureID);
}

void TextureKTX2::Bind()
{
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void TextureKTX2::UnBind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool TextureKTX2::LoadTX2Texture(const char* filePath)
{
	// Verify OpenGL context is available before proceeding
	if (!glfwGetCurrentContext())
	{
		std::cout << "No OpenGL context available for texture loading" << std::endl;
		return false;
	}

	// Check if file exists
	if (!std::filesystem::exists(filePath))
	{
		std::cout << "KTX2 file does not exist: " << filePath << std::endl;
		return false;
	}

	ktxTexture2* texture;
	KTX_error_code result = ktxTexture2_CreateFromNamedFile(
		filePath,
		KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
		&texture
	);
	if (result != KTX_SUCCESS)
	{
		std::cout << "Failed to load KTX2 Texture: " << ktxErrorString(result) << std::endl;
		return false;
	}

	if (ktxTexture2_NeedsTranscoding(texture)) {
		KTX_error_code err = ktxTexture2_TranscodeBasis(
			texture, KTX_TTF_BC3_RGBA, 0);
		if (err != KTX_SUCCESS) {
			std::cout << "Failed to transcode: " << ktxErrorString(err) << std::endl;
			return false;
		}
	}

	// Create an OpenGL texture object
	glGenTextures(1, &textureID);
	glBindTexture(target, textureID);

	result = ktxTexture_GLUpload(reinterpret_cast<ktxTexture*>(texture), &textureID, &target, nullptr);
	if (result != KTX_SUCCESS)
	{
		std::cout << "Failed to upload texture to the GPU: " << ktxErrorString(result) << std::endl;
		ktxTexture_Destroy(reinterpret_cast<ktxTexture*>(texture));
		return false;
	}
	ktxTexture_Destroy(reinterpret_cast<ktxTexture*>(texture));

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return true;

}

bool TextureKTX2::LoadTX2Texture2D(const char* filePath)
{
	// Verify OpenGL context is available before proceeding
	if (!glfwGetCurrentContext())
	{
		std::cout << "No OpenGL context available for texture loading" << std::endl;
		return false;
	}

	// Check if file exists
	if (!std::filesystem::exists(filePath))
	{
		std::cout << "KTX2 file does not exist: " << filePath << std::endl;
		return false;
	}

	ktxTexture2* texture;
	KTX_error_code result = ktxTexture2_CreateFromNamedFile(
		filePath,
		KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
		&texture
	);
	if (result != KTX_SUCCESS)
	{
		std::cout << "Failed to load KTX2 Texture: " << ktxErrorString(result) << std::endl;
		return false;
	}

	if (ktxTexture2_NeedsTranscoding(texture)) {
		KTX_error_code err = ktxTexture2_TranscodeBasis(
			texture, KTX_TTF_BC3_RGBA, 0);
		if (err != KTX_SUCCESS) {
			std::cout << "Failed to transcode: " << ktxErrorString(err) << std::endl;
			return false;
		}
	}

	// Create an OpenGL texture object
	glGenTextures(1, &textureID);
	glBindTexture(target, textureID);

	result = ktxTexture_GLUpload(reinterpret_cast<ktxTexture*>(texture), &textureID, &target, nullptr);
	if (result != KTX_SUCCESS)
	{
		std::cout << "Failed to upload texture to the GPU: " << ktxErrorString(result) << std::endl;
		ktxTexture_Destroy(reinterpret_cast<ktxTexture*>(texture));
		return false;
	}
	ktxTexture_Destroy(reinterpret_cast<ktxTexture*>(texture));

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return true;
}

bool TextureKTX2::ConvertPNGtoKTX2(const std::string& pngSourcePath, const std::string& ktxOutputPath)
{
	// Resolve full paths
	std::filesystem::path inputPath = std::filesystem::absolute(pngSourcePath);
	std::filesystem::path outputPath = std::filesystem::absolute(ktxOutputPath);

	//Validate Inputs:
	if (!std::filesystem::exists(inputPath))
	{
		std::cout << "The input file doesn't exist: " << inputPath << std::endl;
		return false;
	}

	// Resolve toktx path safely
	const char* toktxEnv = std::getenv("TOKTX_PATH");
	std::string toktxPath = toktxEnv ? std::string(toktxEnv) : "toktx";

	// Prepare output directory
	std::filesystem::path outputDir = std::filesystem::path(outputPath).parent_path();
	if (!outputDir.empty() && !std::filesystem::exists(outputDir))
	{
		std::filesystem::create_directories(outputDir);
	}

	// Build a safe command
	std::string cmd = toktxPath;
	cmd += " --bcmp";          // Basis Universal compression << THIS LINE IS CAUSING ERRORS >>
	cmd += " --genmipmap";     // Generate mipmaps
	cmd += " --t2";            // Use KTX2 format
	cmd += " \"" + outputPath.string() + "\"";
	cmd += " \"" + inputPath.string() + "\"";

	//Execute with better error handling
	std::cout << "Executing: " << cmd << std::endl;
	int result = std::system(cmd.c_str());

	// Check results
	if (result != 0)
	{
		std::cout << "Conversion failed with code: " << result << std::endl;
		if (std::filesystem::exists(outputPath))
		{
			std::filesystem::remove(outputPath);
		}
		return false;
	}

	// Verify output
	if (!std::filesystem::exists(outputPath) || std::filesystem::file_size(outputPath) == 0)
	{
		std::cout << "The output file is not created: " << outputPath << std::endl;
		return false;
	}

	return true;
}
