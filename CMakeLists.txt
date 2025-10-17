# CMakeList.txt : CMake project for GetIntoGameDevOPENGL, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
 
project("GetIntoGameDevOPENGL")

add_subdirectory(ENGINE)

# Add source to this project's executable.
add_executable (GetIntoGameDevOPENGL "GetIntoGameDevOPENGL.cpp" "GetIntoGameDevOPENGL.h")

set_property(TARGET GetIntoGameDevOPENGL PROPERTY CXX_STANDARD 20)

target_link_libraries(GetIntoGameDevOPENGL PRIVATE ENGINE)


target_compile_definitions(GetIntoGameDevOPENGL PRIVATE 
	RESOURCES_PATH="${CMAKE_SOURCE_DIR}/ASSETS/"
)

# target_compile_definitions(GetIntoGameDevOPENGL PRIVATE 
# 	RESOURCES_PATH="ASSETS/"
# )

