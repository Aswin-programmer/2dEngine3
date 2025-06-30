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
		(std::string(RESOURCES_PATH) + "SHADER/TESS_TEST/vert.glsl").c_str(),
		(std::string(RESOURCES_PATH) + "SHADER/TESS_TEST/frag.glsl").c_str(),
		(std::string(RESOURCES_PATH) + "SHADER/TESS_TEST/tess_ctrl.glsl").c_str(),
		(std::string(RESOURCES_PATH) + "SHADER/TESS_TEST/tess_eval.glsl").c_str()
	);

	double time = 0;

	float quadVerts[] = {
	-0.5f, -0.5f, // 0 - Bottom Left
	 0.5f, -0.5f, // 1 - Bottom Right
	 0.5f,  0.5f, // 2 - Top Right
	-0.5f,  0.5f  // 3 - Top Left
	};

	GLuint vao, vbo;
	glCreateVertexArrays(1, &vao);

	glCreateBuffers(1, &vbo);
	glNamedBufferStorage(vbo, sizeof(float) * (sizeof(quadVerts) / sizeof(float)), &quadVerts, GL_DYNAMIC_STORAGE_BIT);
	
	glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(float) * 2);

	glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao, 0, 0);
	glEnableVertexArrayAttrib(vao, 0);

	while (!Window::shouldClose())
	{
		time = glfwGetTime();

		Window::clearScreen();
		Window::processInput();

		processKeyInput(Window::getGLFWWindow());

		shader.use();

		textureKTX2.Bind(0);

		// Create projection matrices
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)640 / (float)480, 0.1f, 200.0f);
		shader.setMat4("projection", projection);

		// Camera or View transformation
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);

		/*meshRenderer.CleanUp();

		for (int i = -50; i < 50; i++)
		{
			for (int j = -50; j < 50; j++)
			{
				meshRenderer.AddMesh("CUBE", MeshOrientation(
					glm::vec3(i, j, 0.f),
					glm::vec3(0.f, 0.f, fmod(time, 360)),
					glm::vec3(0.5f, 0.5f, 0.5f)
				));

				meshRenderer.AddMesh("PYRAMID", MeshOrientation(
					glm::vec3(i, j, 1.f),
					glm::vec3(0.f, fmod(time, 360), fmod(time, 360)),
					glm::vec3(0.5f, 0.5f, 0.5f)
				));
			}
		}

		meshRenderer.Render();*/

		shader2.use();
		glBindVertexArray(vao);
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_PATCHES, 0, 4);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
