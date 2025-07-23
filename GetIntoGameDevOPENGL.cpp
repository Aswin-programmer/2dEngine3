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

//// Camera Setup
//Camera camera(glm::vec3(0.f, 0.f, 0.f));
//float lastX = 640.f / 2.f;
//float lastY = 480.f / 2.f;

// Editor Camera Setup
EditorCamera camera(45.0f, 640.f / 480.f, 0.1f, 200.0f);

// Mouse Setup
float GlobalMousePosX = 0.f;
float GlobalMousePosY = 0.f;

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

	Shader shader2 = Shader(
		(std::string(RESOURCES_PATH) + "SHADER/GEO_TEST/geo_vert.glsl").c_str(),
		(std::string(RESOURCES_PATH) + "SHADER/GEO_TEST/geo_frag.glsl").c_str(),
		nullptr,
		nullptr,
		(std::string(RESOURCES_PATH) + "SHADER/GEO_TEST/geo_geo.glsl").c_str()
	);

	Shader shader3 = Shader(
		(std::string(RESOURCES_PATH) + "SHADER/PARTICLE_SYSTEMS/particle_vert.glsl").c_str(),
		(std::string(RESOURCES_PATH) + "SHADER/PARTICLE_SYSTEMS/particle_frag.glsl").c_str(),
		nullptr,
		nullptr,
		(std::string(RESOURCES_PATH) + "SHADER/PARTICLE_SYSTEMS/geometry_particle_shader.glsl").c_str()
	);

	ParticleSystemRenderer particleSystemRenderer = ParticleSystemRenderer();

	double time = 0;

	while (!Window::shouldClose())
	{
		time = glfwGetTime();

		Window::clearScreen();
		Window::processInput();

		processKeyInput(Window::getGLFWWindow());

		shader3.use();

		// Create projection matrices [PERSPECTIVE]
		glm::mat4 projectionP = glm::mat4(1.0f);
		projectionP = glm::perspective(glm::radians(45.0f), (float)640 / (float)480, 0.1f, 200.0f);
		shader3.setMat4("projection", projectionP);

		//// Create projection matrix [ORTHO]
		//glm::mat4 projectionO = glm::mat4(1.0f);
		//projectionO = glm::ortho(0.0f, 640.0f, 0.0f, 480.0f, -1.0f, 1.0f);
		//shader3.setMat4("projection", projectionO);

		// Camera or View transformation
		glm::mat4 view = camera.GetViewMatrix();
		shader3.setMat4("view", view);

		// Model matrix
		glm::mat4 model = glm::mat4{ 1.f };
		//glm::translate(model, glm::vec3(0.f, -15.f, 0.f));
		shader3.setMat4("model", model);

		//particleSystemRenderer.CleanUp();
		particleSystemRenderer.RefreshParticles(Window::getdt());
		particleSystemRenderer.Render();

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
	/*if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
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
	}*/
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	static bool firstMouse = true;
	static float lastX = 0.0f;
	static float lastY = 0.0f;

	if (firstMouse)
	{
		lastX = xposIn;
		lastY = yposIn;
		firstMouse = false;
	}

	float deltaX = xposIn - lastX;
	float deltaY = lastY - yposIn;

	lastX = xposIn;
	lastY = yposIn;

	// Right click = Orbit
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		camera.OnMouseRotate(deltaX, deltaY);
	}
	// Middle click = Pan
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		camera.OnMousePan(deltaX, deltaY);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.OnMouseScroll((float)yoffset);
}
