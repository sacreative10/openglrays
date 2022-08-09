cmake_minimum_required(VERSION 3.0)

project(OpenGLRays)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# only here for folks that rely on compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# add all the third party libraries
add_subdirectory(thirdParty/glfw)
add_subdirectory(thirdParty/glm)
include_directories(thirdParty/glad/include)

set(OPENGL_GL_VERSION "4.6")
set(OPENGL_GLU_VERSION "4.6")
set(OPENGL_GL_PREFER_NEWER_LIBRARIES ON)
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

add_executable(OpenGLRays
thirdParty/glad/glad.c
src/util/logger.h src/util/logger.cpp
src/util/GLutil.h src/util/GLutil.cpp
src/arena.h src/arena.cpp
src/main.cpp
)
target_link_libraries(OpenGLRays glfw OpenGL::GL glm)

add_custom_target(copy_shaders ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/src/shaders
	${CMAKE_CURRENT_BINARY_DIR}/shaders
	COMMENT "Copying shaders"
	)

add_dependencies(OpenGLRays copy_shaders)
