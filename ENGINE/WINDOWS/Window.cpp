#include "Window.h"

// Initialize static members
unsigned int Window::m_width = 0;
unsigned int Window::m_height = 0;
const char* Window::m_title = nullptr;
GLFWwindow* Window::m_window = nullptr;
float Window::beginTime = 0.0f;
float Window::endTime = 0.0f;
float Window::dt = 0.0f;
float Window::r = 0.0f;
float Window::g = 0.0f;
float Window::b = 0.0f;
float Window::a = 1.0f;

bool Window::init(const char* title) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Initialize variables:
    m_width = 640;
    m_height = 480;
    m_title = title;


    // Create the GLFW window
    m_window = glfwCreateWindow(m_width, m_height, m_title, nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    /*glfwSetWindowPos(m_window,
        (mode->width - mode->width) / 2,
        (mode->height - mode->height) / 2
    );*/

    glfwMakeContextCurrent(m_window);
    //glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //Enable v-sync { Locks the FPS to the monitor refreshing rate! }
    glfwSwapInterval(1);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return false;
    }

    // Enable OpenGL features
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set viewport and resize callback
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_POINT_SPRITE);

    // Setting up the face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    // Print OpenGL version info for debugging
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;

    //Find the maximum number of texture units the device supports
    GLint combinedUnits;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &combinedUnits);
    std::cout << "Max combined texture image units (all stages): "
        << combinedUnits << std::endl;

    GLint fragUnits;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &fragUnits);
    std::cout << "Fragment shader texture units: "
        << fragUnits << std::endl;

    GLint vertUnits;
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &vertUnits);
    std::cout << "Vertex shader texture units: "
        << vertUnits << std::endl;

    GLint geomUnits;
    glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &geomUnits);
    std::cout << "Geometry shader texture units: "
        << geomUnits << std::endl;

    GLint tescUnits;
    glGetIntegerv(GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS, &tescUnits);
    std::cout << "Tessellation control shader texture units: "
        << tescUnits << std::endl;

    GLint teseUnits;
    glGetIntegerv(GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS, &teseUnits);
    std::cout << "Tessellation evaluation shader texture units: "
        << teseUnits << std::endl;

    GLint compUnits;
    glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &compUnits);
    std::cout << "Compute shader texture units: "
        << compUnits << std::endl;

    // Set initial viewport
    glViewport(0, 0, m_width, m_height);

    //Delta Time Initialization:
    beginTime = Time::getTime();
    endTime;
    dt = -1.0f;

    return true;
}

void Window::processInput() {
    /*if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_window, true);
    }*/
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(m_window);
}

void Window::update() {

    endTime = Time::getTime();
    dt = endTime - beginTime;
    beginTime = endTime;

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void Window::clearScreen()
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLFWwindow* Window::getGLFWWindow()
{
    return m_window;
}


void Window::getFPS()
{
    std::cout << " " << (1.0f / dt) << "FPS" << std::endl;
}

void Window::cleanup()
{
    glfwTerminate();
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
}


