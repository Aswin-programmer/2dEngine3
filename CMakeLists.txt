# CMakeList.txt : CMake project for GetIntoGameDevOPENGL, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project ("GetIntoGameDevOPENGL")

add_subdirectory(ENGINE)

# Add source to this project's executable.
add_executable (GetIntoGameDevOPENGL "GetIntoGameDevOPENGL.cpp" "GetIntoGameDevOPENGL.h")

set_property(TARGET GetIntoGameDevOPENGL PROPERTY CXX_STANDARD 20)

target_link_libraries(GetIntoGameDevOPENGL PRIVATE ENGINE)
