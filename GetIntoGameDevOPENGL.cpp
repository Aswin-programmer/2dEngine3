// GetIntoGameDevOPENGL.cpp : Defines the entry point for the application.
//

// Force NVIDIA GPU
extern "C" {
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

#include "GetIntoGameDevOPENGL.h"

using namespace std;

int main()
{
	Test test("Hello World");
	test.PrintSample();

	Window::init("TESTING");

	Shader shader((std::string(RESOURCES_PATH) + "SHADER/test.vert").c_str(),
		(std::string(RESOURCES_PATH) + "SHADER/test.frag").c_str());

	TextureKTX2 textureKTX2;
	textureKTX2.ConvertPNGtoKTX2(std::string(RESOURCES_PATH) + "TEXTURE/PNG/awesomeface.png",
		std::string(RESOURCES_PATH) + "TEXTURE/KTX/awesomeface.ktx2");
	textureKTX2.LoadTX2Texture((std::string(RESOURCES_PATH) + "TEXTURE/KTX/awesomeface.ktx2").c_str());

	TextureKTX2 tex1;
	tex1.LoadTX2Texture2D((std::string(RESOURCES_PATH) + "TEXTURE/KTX/awesomeface2D.ktx2").c_str());

	float vertices[] = {
		// positions   // uvs
		-1, -1,        0.0f, 0.0f,
		 1, -1,        1.0f, 0.0f,
		 1,  1,        1.0f, 1.0f,
		-1,  1,        0.0f, 1.0f,
	};
	GLuint indices[] = { 0, 1, 2, 2, 3, 0 };

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind VAO
	glBindVertexArray(VAO);

	// Bind and upload vertex data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind and upload index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	// Vertex UV attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// Unbind to prevent accidental changes (optional, but good practice)
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	shader.use();
	glActiveTexture(GL_TEXTURE0);
	shader.setInt("tex", 0);

	float zoom = 1.0f;

	int layer = 0;

	while (!Window::shouldClose())
	{
		Window::clearScreen();
		Window::processInput();

		if (glfwGetKey(Window::getGLFWWindow(), GLFW_KEY_UP) == GLFW_PRESS) zoom *= 1.01f;
		if (glfwGetKey(Window::getGLFWWindow(), GLFW_KEY_DOWN) == GLFW_PRESS) zoom /= 1.01f;

		shader.use();
		shader.setFloat("zoom", zoom);

		static bool wWasPressed = false;
		bool wIsPressed = glfwGetKey(Window::getGLFWWindow(), GLFW_KEY_W) == GLFW_PRESS;

		if (wIsPressed && !wWasPressed) {
			layer = (layer + 1) % 4;
		}
		wWasPressed = wIsPressed;

		shader.setInt("layer", layer);

		glActiveTexture(GL_TEXTURE0);
		textureKTX2.Bind();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		Window::update();
	}
	Window::cleanup();
	return 0;
}
