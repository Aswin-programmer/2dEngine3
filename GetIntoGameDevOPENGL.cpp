// GetIntoGameDevOPENGL.cpp : Defines the entry point for the application.
//

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

// Force NVIDIA GPU
extern "C" {

	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
#define M_PI 3.14159265358979323846

#define NOMINMAX

#include "GetIntoGameDevOPENGL.h"
#include <RENDERER/TEXT/TextRenderer.h>

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

reactphysics3d::Vector3 ToEulerAngles(const reactphysics3d::Quaternion& q) {
    reactphysics3d::Vector3 angles;

    // roll (x-axis rotation)
    float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    angles.x = std::atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    float sinp = 2 * (q.w * q.y - q.z * q.x);
    if (std::abs(sinp) >= 1)
        angles.y = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
    else
        angles.y = std::asin(sinp);

    // yaw (z-axis rotation)
    float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    angles.z = std::atan2(siny_cosp, cosy_cosp);

    return angles;
}


int main()
{
	Test test("Hello World");
	test.PrintSample();

	Window::init("TESTING");

	//Setup the MemoryTracker
	std::shared_ptr<MemoryTracker> memoryTracker(std::make_shared<MemoryTracker>());

	glfwSetCursorPosCallback(Window::getGLFWWindow(), mouse_callback);
	glfwSetScrollCallback(Window::getGLFWWindow(), scroll_callback);

	Shader shader = Shader((std::string(RESOURCES_PATH) + "SHADER/cube.vert").c_str()
		, (std::string(RESOURCES_PATH) + "SHADER/cube.frag").c_str());

	TextureKTX2 textureKTX2 = TextureKTX2((std::string(RESOURCES_PATH) + "TEXTURE/KTX/cube.ktx2").c_str(), TextureType::NORMAL{});

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

	memoryTracker->AddMemoryTrackerEntry("Test1");

	//// Temporary setup for the GLTFMESHRenderer
	//GLTFMESHRenderer gltfMeshRenderer = GLTFMESHRenderer(std::string(RESOURCES_PATH) + "GLTFMODEL/MIXED_MODEL/mix.gltf");
	Shader shader4 = Shader((std::string(RESOURCES_PATH) + "SHADER/GLTF_MODEL/gltf_vert.glsl").c_str()
		, (std::string(RESOURCES_PATH) + "SHADER/GLTF_MODEL/gltf_frag.glsl").c_str());
	shader4.SetupDefaultTextureSlots();
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

	memoryTracker->AddEndForMemoryTrackerEntry("Test1");

	GLTFMESHRenderer gltfRenderer;
	gltfRenderer.AddGLTFModelToRenderer(std::string("mix.gltf"), GLTFModelOrientation(
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(1.f, 1.f, 1.f),
		-1
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

	TextureKTX2 skyboxTexture = TextureKTX2((std::string(RESOURCES_PATH) + "TEXTURE/CUBEMAP/skybox.ktx2").c_str(), TextureType::CUBEMAP{});

	//    ##          PHYSICS SECTION          ## 
	
	memoryTracker->AddMemoryTrackerEntry("Physics");
	 
	reactphysics3d::PhysicsCommon physicsCommon;

	reactphysics3d::PhysicsWorld* world = physicsCommon.createPhysicsWorld();

	// Falling cube
	reactphysics3d::RigidBody* body = world->createRigidBody(reactphysics3d::Transform(reactphysics3d::Vector3(0, 20, 0), reactphysics3d::Quaternion::identity()));
	body->setType(reactphysics3d::BodyType::DYNAMIC);

	reactphysics3d::BoxShape* boxShape = physicsCommon.createBoxShape(reactphysics3d::Vector3(0.5f, 0.5f, 0.5f));
	body->addCollider(boxShape, reactphysics3d::Transform::identity());
	body->updateMassPropertiesFromColliders();

	// Ground
	reactphysics3d::RigidBody* ground = world->createRigidBody(reactphysics3d::Transform(reactphysics3d::Vector3(0, 0.f, 0), reactphysics3d::Quaternion::identity()));
	ground->setType(reactphysics3d::BodyType::STATIC);

	reactphysics3d::BoxShape* groundShape = physicsCommon.createBoxShape(reactphysics3d::Vector3(10.f, 0.5f, 10.f));
	ground->addCollider(groundShape, reactphysics3d::Transform::identity());

	memoryTracker->AddEndForMemoryTrackerEntry("Physics");

	// Step physics
	const float timeStep = 1.0f / 60.0f;


	// ##                       End of physics section                ##

	// ##      Text Renderering ##
	
	std::shared_ptr<TextRenderer> textRenderer = std::make_shared<TextRenderer>(
		(std::string(RESOURCES_PATH) + "FONTS/DEBROSEE/DEBROSEE.ttf"),
		Window::getWidth(),
		Window::getHeight()
	);
	textRenderer->InitTextRenderer();
	
	// ## END OF TEXT RENDERING ##

	while (!Window::shouldClose())
	{
		//PROFILE_SCOPE_N("MainLoop");

		memoryTracker->AddMemoryTrackerEntry("Rendering");

		time = glfwGetTime();

		processKeyInput(Window::getGLFWWindow());

		Window::clearScreen();
		Window::processInput();

		

	//	shader3.use();

	//	// Create projection matrices [PERSPECTIVE]
	//	glm::mat4 projectionP = glm::mat4(1.0f);
	//	projectionP = glm::perspective(glm::radians(45.0f), (float)640 / (float)480, 0.1f, 200.0f);
	//	shader3.setMat4("projection", projectionP);

	//	//// Create projection matrix [ORTHO]
	//	//glm::mat4 projectionO = glm::mat4(1.0f);
	//	//projectionO = glm::ortho(0.0f, 640.0f, 0.0f, 480.0f, -1.0f, 1.0f);
	//	//shader3.setMat4("projection", projectionO);

	//	// Camera or View transformation
	//	glm::mat4 view = camera.GetViewMatrix();
	//	shader3.setMat4("view", view);

	//	// Model matrix
	//	glm::mat4 model = glm::mat4{ 1.f };
	//	//glm::translate(model, glm::vec3(0.f, -15.f, 0.f));
	//	shader3.setMat4("model", model);

	//	//particleSystemRenderer.CleanUp();
	//	particleSystemRenderer.RefreshParticles(Window::getdt());
	//	particleSystemRenderer.Render();

	//	glm::mat4 vp = projectionP * view;
	//	Frustum frustum = ExtractFrustumPlanes(vp);

	//	glm::vec3 objPos = glm::vec3(0.f, 0.f, 0.f);
	//	float objRadius = 5.0f; // precomputed or loaded from model bounds

	//	

	//	// --- Safe skybox draw (do this BEFORE drawing scene meshes or AFTER with depth mask off) ---

	//	// Save minimal state (optional, but explicit)
	//	GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
	//	GLboolean wasDepthTest = glIsEnabled(GL_DEPTH_TEST);
	//	GLint prevDepthFunc;
	//	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFunc);

	//	// Prepare state for skybox
	//	glDisable(GL_CULL_FACE);                // draw inside faces easily
	//	glDepthFunc(GL_LEQUAL);                 // allow skybox fragments at far plane
	//	glDepthMask(GL_FALSE);                  // IMPORTANT: don't write to depth buffer

	//	skyboxShader.use();
	//	// Build pure rotation from yaw/pitch
	//	glm::mat4 pureRotation = glm::mat4(1.0f);
	//	pureRotation = glm::rotate(pureRotation, glm::radians(camera.GetPitch()), glm::vec3(1, 0, 0));
	//	pureRotation = glm::rotate(pureRotation, glm::radians(camera.GetYaw()), glm::vec3(0, 1, 0));

	//	skyboxShader.setMat4("u_view", pureRotation);
	//	skyboxShader.setMat4("u_proj", projectionP);

	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture.GetTextureID());
	//	skyboxShader.setInt("skybox", 0);

	//	glBindVertexArray(skyboxVAO);
	//	glDrawArrays(GL_TRIANGLES, 0, 36);
	//	glBindVertexArray(0);

	//	// Restore state
	//	glDepthMask(GL_TRUE);                   // allow meshes to write depth again
	//	glDepthFunc(GL_LESS);                   // restore typical depth test
	//	if (!wasCull) glDisable(GL_CULL_FACE);  // optional restore; ensure CullFace is consistent
	//	else glEnable(GL_CULL_FACE);
	//	// (if you need to restore other state like glFrontFace/glCullFace, do it here)

	//	
	//	gltfRenderer.CleanUp();

	//	//gltfRenderer.AddGLTFModelToRenderer(std::string("mix.gltf"), GLTFModelOrientation(
	//	//	glm::vec3(0.f, 0.f, 0.f),
	//	//	glm::vec3(0.f, 0.f, 0.f),
	//	//	glm::vec3(1.f, 1.f, 1.f)
	//	//));
	//	//gltfRenderer.AddGLTFModelToRenderer(std::string("cube.gltf"), GLTFModelOrientation(
	//	//	glm::vec3(1.f, 1.f, 0.f),
	//	//	glm::vec3(0.f, 0.f, 0.f),
	//	//	glm::vec3(1.f, 1.f, 1.f)
	//	//));
	//	//gltfRenderer.AddGLTFModelToRenderer(std::string("Avocado.gltf"), GLTFModelOrientation(
	//	//	glm::vec3(3.f, 1.f, 0.f),
	//	//	glm::vec3(0.f, 0.f, 0.f),
	//	//	glm::vec3(50.f, 50.f, 50.f)
	//	//));
	//	//gltfRenderer.AddGLTFModelToRenderer(std::string("BarramundiFish.gltf"), GLTFModelOrientation(
	//	//	glm::vec3(3.f, 3.f, 0.f),
	//	//	glm::vec3(90.f, 0.f, 0.f),
	//	//	glm::vec3(50.f, 50.f, 50.f)
	//	//));
	//	//gltfRenderer.AddGLTFModelToRenderer(std::string("Avocado.gltf"), GLTFModelOrientation(
	//	//	glm::vec3(-3.f, -1.f, 0.f),
	//	//	glm::vec3(0.f, 0.f, 0.f),
	//	//	glm::vec3(50.f, 50.f, 50.f)
	//	//));
	//	//gltfRenderer.AddGLTFModelToRenderer(std::string("BarramundiFish.gltf"), GLTFModelOrientation(
	//	//	glm::vec3(-3.f, -3.f, 0.f),
	//	//	glm::vec3(90.f, 0.f, 0.f),
	//	//	glm::vec3(50.f, 50.f, 50.f)
	//	//));

	//	//float time1 = glfwGetTime();
	//	//float rotate = fmod(time * 50.0f, 360.0f); // rotate at 50 deg/sec

	//	//for (int i = -50; i <= 50; i++)
	//	//{
	//	//	for (int j = -50; j <= 50; j++)
	//	//	{
	//	//		glm::vec3 objPos = glm::vec3(i, 0.f, j);
	//	//		//if (IsSphereInsideFrustum(frustum, objPos, objRadius)) {
	//	//		//	gltfRenderer.AddGLTFModelToRenderer("BarramundiFish.gltf", GLTFModelOrientation(
	//	//		//		objPos,
	//	//		//		glm::vec3(
	//	//		//			0.f,                                   // X rotation (pitch)
	//	//		//			fmod(time * 50.0f + (i + j) * 10.0f, 360.0f), // Y rotation (yaw) with offset per grid position
	//	//		//			0.f                                    // Z rotation (roll)
	//	//		//		),
	//	//		//		glm::vec3(5.f)
	//	//		//	));
	//	//		//}
	//	//		gltfRenderer.AddGLTFModelToRenderer("BarramundiFish.gltf", GLTFModelOrientation(
	//	//			objPos,
	//	//			glm::vec3(
	//	//				0.f,                                   // X rotation (pitch)
	//	//				fmod(time1 * 50.0f + (i + j) * 10.0f, 360.0f), // Y rotation (yaw) with offset per grid position
	//	//				0.f                                    // Z rotation (roll)
	//	//			),
	//	//			glm::vec3(5.f)
	//	//		));
	//	//	}
	//	//}

	//	/*gltfRenderer.ExperimentalHelper();
	//	
	//
	//	shader4.use();
	//	shader4.setMat4("view", view);
	//	shader4.setMat4("projection", projectionP);
	//	gltfRenderer.GLTFMESHRender(shader4);*/

	//	
	//	

	//	reactphysics3d::Transform transform = body->getTransform();
	//	reactphysics3d::Vector3 position = transform.getPosition();
	//	reactphysics3d::Quaternion q = transform.getOrientation();
	//	auto euler = ToEulerAngles(q);
	//	gltfRenderer.AddGLTFModelToRenderer(std::string("cube.gltf"), GLTFModelOrientation(
	//		glm::vec3(position.x, position.y, position.z),
	//		glm::vec3(glm::degrees(euler.x), glm::degrees(euler.y), glm::degrees(euler.z)),
	//		glm::vec3(1.f, 1.f, 1.f),
	//		-1
	//	));

	//	reactphysics3d::Transform transform2 = ground->getTransform();
	//	reactphysics3d::Vector3 position2 = transform2.getPosition();
	//	q = transform2.getOrientation();
 //   	euler = ToEulerAngles(q);
	//	reactphysics3d::Collider* collider = ground->getCollider(0);
	//	reactphysics3d::BoxShape* shape = dynamic_cast<reactphysics3d::BoxShape*>(collider->getCollisionShape());

	//	if(shape) {
	//		reactphysics3d::Vector3 halfExtents = shape->getHalfExtents();
	//		reactphysics3d::Vector3 size = halfExtents;  // full size

	//		gltfRenderer.AddGLTFModelToRenderer(
	//			"cube.gltf",
	//			GLTFModelOrientation(
	//				glm::vec3(position2.x, position2.y, position2.z),
	//				glm::vec3(glm::degrees(euler.x), glm::degrees(euler.y), glm::degrees(euler.z)),            // rotation (you can also extract quaternion if needed)
	//				glm::vec3(size.x, size.y, size.z),    // scale == size
	//				-1
	//			)
	//		);
	//	}

	//	gltfRenderer.AddGLTFModelToRenderer(std::string("BarramundiFish.gltf"), GLTFModelOrientation(
	//		glm::vec3(3.f, 3.f, 0.f),
	//		glm::vec3(90.f, 0.f, 0.f),
	//		glm::vec3(50.f, 50.f, 50.f),
	//		-1
	//	));

	//	gltfRenderer.AddGLTFModelToRenderer(std::string("Avocado.gltf"), GLTFModelOrientation(
	//		glm::vec3(-3.f, -1.f, 0.f),
	//		glm::vec3(0.f, 0.f, 0.f),
	//		glm::vec3(50.f, 50.f, 50.f),
	//		-1
	//	));


	//	gltfRenderer.ExperimentalHelper();
	//	
	//
	//	shader4.use();
	//	shader4.setMat4("view", view);
	//	shader4.setMat4("projection", projectionP);
	//	gltfRenderer.GLTFMESHRender(shader4);

	//	gltfRenderer.CleanUp();


	//	world->update(timeStep);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		textRenderer->TextRendererDraw("Hello GUI!", 100, 200, 1.0f, 1.0f, 1.0f, 1.0f, GLYPH_NONE);

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);


		Window::update();

		memoryTracker->AddEndForMemoryTrackerEntry("Rendering");

		memoryTracker->PrintCurrentMemoryUsage();
		memoryTracker->PrettyPrintMemoryAllocationForTrackers();

		

		std::cout << "The FPS is : " << Window::GetFPSValue() << std::endl;

		//PROFILE_FRAME();
	}

	GLTFMESHLoader::ClearAllGLTFModels();
	
	Window::cleanup();

	physicsCommon.destroyPhysicsWorld(world);

	_CrtDumpMemoryLeaks();

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
