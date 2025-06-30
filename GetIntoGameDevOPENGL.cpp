// GetIntoGameDevOPENGL.cpp : Defines the entry point for the application.
//

// Force NVIDIA GPU
extern "C" {

	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

#include "GetIntoGameDevOPENGL.h"

// Opengl Callbacks
void processKeyInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Camera Setup
Camera camera(glm::vec3(0.f, 0.f, 3.f));
float lastX = 640.f / 2.f;
float lastY = 480.f / 2.f;

int main()
{
	Test test("Hello World");
	test.PrintSample();

	Window::init("TESTING");

	glfwSetCursorPosCallback(Window::getGLFWWindow(), mouse_callback);
	glfwSetScrollCallback(Window::getGLFWWindow(), scroll_callback);

	Shader shader = Shader((std::string(RESOURCES_PATH) + "SHADER/cube.vert").c_str()
		, (std::string(RESOURCES_PATH) + "SHADER/cube.frag").c_str());

	TextureKTX2 textureKTX2 = TextureKTX2((std::string(RESOURCES_PATH) + "TEXTURE/KTX/cube.ktx2").c_str());
	shader.use();
	shader.setInt("u_Diffuse", 0);

	BasicMeshRendererInstantiated meshRenderer = BasicMeshRendererInstantiated();

	Shader shader2 = Shader(
		(std::string(RESOURCES_PATH) + "SHADER/TERRAIN_TEST/vert.glsl").c_str(),
		(std::string(RESOURCES_PATH) + "SHADER/TERRAIN_TEST/frag.glsl").c_str(),
		(std::string(RESOURCES_PATH) + "SHADER/TERRAIN_TEST/tess_ctrl.glsl").c_str(),
		(std::string(RESOURCES_PATH) + "SHADER/TERRAIN_TEST/tess_eval.glsl").c_str()
	);

	TextureKTX2 terrain_height_map = TextureKTX2((std::string(RESOURCES_PATH) + "TEXTURE/KTX/terrain_height_map.ktx2").c_str());
	TextureKTX2 terrain_texture = TextureKTX2((std::string(RESOURCES_PATH) + "TEXTURE/KTX/terrain2.ktx2").c_str());

	GLuint vao;
	glCreateVertexArrays(1, &vao);

	double time = 0;

	while (!Window::shouldClose())
	{
		time = glfwGetTime();

		Window::clearScreen();
		Window::processInput();

		processKeyInput(Window::getGLFWWindow());

		glBindVertexArray(vao);

		terrain_height_map.Bind(0);
		terrain_texture.Bind(1);

		shader2.use();

		shader2.setFloat("dmap_depth", 5.0f);
		shader2.setInt("tex_displacement", 0);
		shader2.setInt("tex_color", 1);

		// Create projection matrices
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)640 / (float)480, 0.1f, 200.0f);
		shader2.setMat4("projection", projection);

		// Camera or View transformation
		glm::mat4 view = camera.GetViewMatrix();
		shader2.setMat4("view", view);

		// Model matrix
		glm::mat4 model = glm::mat4{ 1.f };
		//glm::translate(model, glm::vec3(0.f, -15.f, 0.f));
		shader2.setMat4("model", model);

		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);

		Window::update();
	}
	Window::cleanup();
	return 0;
}

void processKeyInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(FORWARD, Window::getdt());
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(BACKWARD, Window::getdt());
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(LEFT, Window::getdt());
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(RIGHT, Window::getdt());
	}
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
