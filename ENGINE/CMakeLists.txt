# CMakeList.txt : CMake project for ENGINE, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("ENGINE")

add_subdirectory(DEPENDENCIES/GLFW)
add_subdirectory(DEPENDENCIES/GLAD)
add_subdirectory(DEPENDENCIES/GLM)
add_subdirectory(DEPENDENCIES/KTX)

# Add source to this project's library.
add_library (ENGINE 
	"ENGINE.cpp" 
	"ENGINE.h" 
	"WINDOWS/Window.h" 
	"WINDOWS/Window.cpp" 
	"UTILITIES/Time.h" 
	"UTILITIES/Time.cpp" 
	"RENDERER/TEXTURE_KTX/TextureKTX2.h"
	"RENDERER/TEXTURE_KTX/TextureKTX2.cpp"
	"RENDERER/SHADERS/Shader.h" 
	"RENDERER/SHADERS/Shader.cpp" 
	"CAMERA/Camera.h" 
	"CAMERA/Camera.cpp" 
	"RENDERER/TEXTURE_KTX/TextureLoader.h" 
	"RENDERER/TEXTURE_KTX/TextureLoader.cpp" 
	"RENDERER/MESH/BASICMESH/BasicMeshRendererInstantiated.h"
	"RENDERER/MESH/BASICMESH/BasicMeshRendererInstantiated.cpp"
 "RENDERER/BEZIERS_CURVE/BezierCurveRenderer.h" "RENDERER/BEZIERS_CURVE/BezierCurveRenderer.cpp")

set_property(TARGET ENGINE PROPERTY CXX_STANDARD 20)

target_include_directories(ENGINE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
 
target_link_libraries(ENGINE PUBLIC 
	glfw 
	glad 
	glm
	ktx
)


