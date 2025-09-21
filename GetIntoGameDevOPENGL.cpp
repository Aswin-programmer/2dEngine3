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

struct Frustum {
	glm::vec4 planes[6]; // (a, b, c, d) for ax+by+cz+d=0
};

Frustum ExtractFrustumPlanes(const glm::mat4& vp) {
	Frustum frustum;

	// Left
	frustum.planes[0] = glm::vec4(
		vp[0][3] + vp[0][0],
		vp[1][3] + vp[1][0],
		vp[2][3] + vp[2][0],
		vp[3][3] + vp[3][0]
	);
	// Right
	frustum.planes[1] = glm::vec4(
		vp[0][3] - vp[0][0],
		vp[1][3] - vp[1][0],
		vp[2][3] - vp[2][0],
		vp[3][3] - vp[3][0]
	);
	// Bottom
	frustum.planes[2] = glm::vec4(
		vp[0][3] + vp[0][1],
		vp[1][3] + vp[1][1],
		vp[2][3] + vp[2][1],
		vp[3][3] + vp[3][1]
	);
	// Top
	frustum.planes[3] = glm::vec4(
		vp[0][3] - vp[0][1],
		vp[1][3] - vp[1][1],
		vp[2][3] - vp[2][1],
		vp[3][3] - vp[3][1]
	);
	// Near
	frustum.planes[4] = glm::vec4(
		vp[0][3] + vp[0][2],
		vp[1][3] + vp[1][2],
		vp[2][3] + vp[2][2],
		vp[3][3] + vp[3][2]
	);
	// Far
	frustum.planes[5] = glm::vec4(
		vp[0][3] - vp[0][2],
		vp[1][3] - vp[1][2],
		vp[2][3] - vp[2][2],
		vp[3][3] - vp[3][2]
	);

	// Normalize all planes
	for (int i = 0; i < 6; i++) {
		float length = glm::length(glm::vec3(frustum.planes[i]));
		frustum.planes[i] /= length;
	}

	return frustum;
}

bool IsSphereInsideFrustum(const Frustum& frustum, const glm::vec3& center, float radius) {
	for (int i = 0; i < 6; i++) {
		float distance = glm::dot(glm::vec3(frustum.planes[i]), center) + frustum.planes[i].w;
		if (distance < -radius)
			return false; // Completely outside
	}
	return true; // At least partially inside
}

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

	//// Temporary setup for the GLTFMESHRenderer
	//GLTFMESHRenderer gltfMeshRenderer = GLTFMESHRenderer(std::string(RESOURCES_PATH) + "GLTFMODEL/MIXED_MODEL/mix.gltf");
	Shader shader4 = Shader((std::string(RESOURCES_PATH) + "SHADER/GLTF_MODEL/gltf_vert.glsl").c_str()
		, (std::string(RESOURCES_PATH) + "SHADER/GLTF_MODEL/gltf_frag.glsl").c_str());

	if (!GLTFMESHLoader::LoadGLTFModel(std::string(RESOURCES_PATH) + "GLTFMODEL/MIXED_MODEL/mix.gltf"))
	{
		std::cout << "Failed to load the sample model!\n";
	}
	if (!GLTFMESHLoader::LoadGLTFModel(std::string(RESOURCES_PATH) + "GLTFMODEL/CUBE/cube.gltf"))
	{
		std::cout << "Failed to load the sample model!\n";
	}
	if (!GLTFMESHLoader::LoadGLTFModel(std::string(RESOURCES_PATH) + "GLTFMODEL/AVOCADO/Avocado.gltf"))
	{
		std::cout << "Failed to load the sample model!\n";
	}
	if (!GLTFMESHLoader::LoadGLTFModel(std::string(RESOURCES_PATH) + "GLTFMODEL/FISH/BarramundiFish.gltf"))
	{
		std::cout << "Failed to load the sample model!\n";
	}
	GLTFMESHRenderer gltfRenderer;
	gltfRenderer.AddGLTFModelToRenderer(std::string("mix.gltf"), GLTFModelOrientation(
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(1.f, 1.f, 1.f)
	));


	// Testing the working of the skybox:
	// cube vertices (36 vertices: 6 faces * 2 tris * 3 verts)
	float skyboxVerts[] = {
		// positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVerts), &skyboxVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	Shader skyboxShader = Shader((std::string(RESOURCES_PATH) + "SHADER/CUBE_MAP/cube_map_vert.glsl").c_str()
		, (std::string(RESOURCES_PATH) + "SHADER/CUBE_MAP/cube_map_frag.glsl").c_str());

	TextureKTX2 skyboxTexture = TextureKTX2((std::string(RESOURCES_PATH) + "TEXTURE/CUBEMAP/skybox.ktx2").c_str());

	while (!Window::shouldClose())
	{
		PROFILE_SCOPE_N("MainLoop");

		time = glfwGetTime();

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

		glm::mat4 vp = projectionP * view;
		Frustum frustum = ExtractFrustumPlanes(vp);

		glm::vec3 objPos = glm::vec3(0.f, 0.f, 0.f);
		float objRadius = 5.0f; // precomputed or loaded from model bounds

		Window::clearScreen();
		Window::processInput();

		// --- Safe skybox draw (do this BEFORE drawing scene meshes or AFTER with depth mask off) ---

		// Save minimal state (optional, but explicit)
		GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
		GLboolean wasDepthTest = glIsEnabled(GL_DEPTH_TEST);
		GLint prevDepthFunc;
		glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFunc);

		// Prepare state for skybox
		glDisable(GL_CULL_FACE);                // draw inside faces easily
		glDepthFunc(GL_LEQUAL);                 // allow skybox fragments at far plane
		glDepthMask(GL_FALSE);                  // IMPORTANT: don't write to depth buffer

		skyboxShader.use();
		// Build pure rotation from yaw/pitch
		glm::mat4 pureRotation = glm::mat4(1.0f);
		pureRotation = glm::rotate(pureRotation, glm::radians(camera.GetPitch()), glm::vec3(1, 0, 0));
		pureRotation = glm::rotate(pureRotation, glm::radians(camera.GetYaw()), glm::vec3(0, 1, 0));

		skyboxShader.setMat4("u_view", pureRotation);
		skyboxShader.setMat4("u_proj", projectionP);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture.GetTextureID());
		skyboxShader.setInt("skybox", 0);

		glBindVertexArray(skyboxVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		// Restore state
		glDepthMask(GL_TRUE);                   // allow meshes to write depth again
		glDepthFunc(GL_LESS);                   // restore typical depth test
		if (!wasCull) glDisable(GL_CULL_FACE);  // optional restore; ensure CullFace is consistent
		else glEnable(GL_CULL_FACE);
		// (if you need to restore other state like glFrontFace/glCullFace, do it here)

		gltfRenderer.CleanUp();

		gltfRenderer.AddGLTFModelToRenderer(std::string("mix.gltf"), GLTFModelOrientation(
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(1.f, 1.f, 1.f)
		));
		gltfRenderer.AddGLTFModelToRenderer(std::string("cube.gltf"), GLTFModelOrientation(
			glm::vec3(1.f, 1.f, 0.f),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(1.f, 1.f, 1.f)
		));
		gltfRenderer.AddGLTFModelToRenderer(std::string("Avocado.gltf"), GLTFModelOrientation(
			glm::vec3(3.f, 1.f, 0.f),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(50.f, 50.f, 50.f)
		));
		gltfRenderer.AddGLTFModelToRenderer(std::string("BarramundiFish.gltf"), GLTFModelOrientation(
			glm::vec3(3.f, 3.f, 0.f),
			glm::vec3(90.f, 0.f, 0.f),
			glm::vec3(50.f, 50.f, 50.f)
		));
		gltfRenderer.AddGLTFModelToRenderer(std::string("Avocado.gltf"), GLTFModelOrientation(
			glm::vec3(-3.f, -1.f, 0.f),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(50.f, 50.f, 50.f)
		));
		gltfRenderer.AddGLTFModelToRenderer(std::string("BarramundiFish.gltf"), GLTFModelOrientation(
			glm::vec3(-3.f, -3.f, 0.f),
			glm::vec3(90.f, 0.f, 0.f),
			glm::vec3(50.f, 50.f, 50.f)
		));

		float time1 = glfwGetTime();
		float rotate = fmod(time * 50.0f, 360.0f); // rotate at 50 deg/sec

		for (int i = -50; i <= 50; i++)
		{
			for (int j = -50; j <= 50; j++)
			{
				glm::vec3 objPos = glm::vec3(i, 0.f, j);
				//if (IsSphereInsideFrustum(frustum, objPos, objRadius)) {
				//	gltfRenderer.AddGLTFModelToRenderer("BarramundiFish.gltf", GLTFModelOrientation(
				//		objPos,
				//		glm::vec3(
				//			0.f,                                   // X rotation (pitch)
				//			fmod(time * 50.0f + (i + j) * 10.0f, 360.0f), // Y rotation (yaw) with offset per grid position
				//			0.f                                    // Z rotation (roll)
				//		),
				//		glm::vec3(5.f)
				//	));
				//}
				gltfRenderer.AddGLTFModelToRenderer("BarramundiFish.gltf", GLTFModelOrientation(
					objPos,
					glm::vec3(
						0.f,                                   // X rotation (pitch)
						fmod(time1 * 50.0f + (i + j) * 10.0f, 360.0f), // Y rotation (yaw) with offset per grid position
						0.f                                    // Z rotation (roll)
					),
					glm::vec3(5.f)
				));
			}
		}

		gltfRenderer.ExperimentalHelper();
		
	
		shader4.use();
		shader4.setMat4("view", view);
		shader4.setMat4("projection", projectionP);
		gltfRenderer.GLTFMESHRender(shader4);

		Window::update();

		PROFILE_FRAME();
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
