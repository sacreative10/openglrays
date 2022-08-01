cmake_minimum_required(VERSION 3.0)

project(GLFWStarter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# only here for folks that rely on compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# add all the third party libraries
add_subdirectory(thirdParty/glfw)
include_directories(thirdParty/glad/include)

set(OPENGL_GL_VERSION "4.6")
set(OPENGL_GLU_VERSION "4.6")
set(OPENGL_GL_PREFER_NEWER_LIBRARIES ON)
set(OPENGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

add_executable(GLFWStarter thirdParty/glad/glad.c src/main.cpp src/util/logger.cpp src/util/GLutil.cpp)
target_link_libraries(GLFWStarter glfw OpenGL::GL)

add_custom_target(copy_shaders ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/src/shaders
	${CMAKE_CURRENT_BINARY_DIR}/shaders
	COMMENT "Copying shaders"
	)

add_dependencies(GLFWStarter copy_shaders)
