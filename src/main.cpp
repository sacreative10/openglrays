#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "util/GLutil.h"
#include "util/logger.h"


#include "arena.h"
using namespace Arena;


const int OPENGL_MAJOR_VERSION = 4;
const int OPENGL_MINOR_VERSION = 6;

const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

GLfloat Vertices[] =
{
	1.0f,  1.0f, 0.0f,  // top right 
	1.0f, -1.0f, 0.0f,  // bottom right
	-1.0f, -1.0f, 0.0f,  // bottom left
	-1.0f,  1.0f, 0.0f
};

GLuint Indices[] =
{
	0, 1, 3,
	1, 2, 3
};

const GLfloat UVs[] = {
	0.0F, 0.0F,
	1.0F, 0.0F,
	1.0F, 1.0F,
	1.0F, 1.0F,
	0.0F, 1.0F,
	0.0F, 0.0F,
};


bool vSync = true;

void error_callback(int error, const char* description)
{
	logger::Log(logger::LogLevel::ERROR, std::string("GLFW error: ") + description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	logger::Log(logger::LogLevel::INFO, std::string("Framebuffer size changed to ") + std::to_string(width) + "x" + std::to_string(height));
	glViewport(0, 0, width, height);
}

void input_callback(GLFWwindow* window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


// This function generates and binds all the objects arrays. Edit this to your liking.
std::vector<GLuint> alltheobjects()
{
	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	return std::vector<GLuint>{VAO, VBO, EBO};
}

void placeBasicScene() {
	objects.push_back(Object{ { 0.0f, 1.0f, 0.0f }, 0.5f, Material{ { 0.5f, 0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 0.0f, 0.0f } });
	lights.push_back(Light{ { 0.0f, 5.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, 0.5f, 1.0f, 100.0f });
	planeMaterial = Material({ 0.5F, 0.5F, 0.5F }, { 0.75F, 0.75F, 0.75F }, { 0.0F, 0.0F, 0.0F }, 0.0F, 0.0F);
}


void handleMovement(GLFWwindow* window, glm::vec3 cameraPosition, float cameraYaw, float cameraPitch, glm::mat4* rotationMatrix)
{
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	//glfwSetCursorPos(window, SCR_WIDTH / 2, SCR_HEIGHT / 2);

	float xoffset = (float)mouseX - SCR_WIDTH / 2;
	float yoffset = (float)mouseY - SCR_HEIGHT / 2;

	cameraYaw += xoffset * 0.01f;
	cameraPitch += yoffset * 0.01f;

	if (cameraPitch > 89.0f)
		cameraPitch = 89.0f;
	if (cameraPitch < -89.0f)
		cameraPitch = -89.0f;

	*rotationMatrix = glm::rotate(glm::rotate(glm::mat4(1), cameraPitch, glm::vec3(1, 0, 0)), cameraYaw, glm::vec3(0, 1, 0));
	
	glm::vec3 forward = glm::vec3(glm::vec4(0, 0, -1, 0) * (*rotationMatrix));

	glm::vec3 up(0, 1, 0);
	glm::vec3 right = glm::cross(forward, up);

	glm::vec3 movementDir = glm::vec3(0, 0, 0);
	float multiplier = 1.0f;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		movementDir += forward;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		movementDir -= forward;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		movementDir -= right;	
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		movementDir += right;
	if (glfwGetKey(window, GLFW_KEY_SPACE)) 
		movementDir += up;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
		movementDir -= up;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) 
		multiplier = 5;
	
	if(glm::length(movementDir) > 0.0f)
	{
		cameraPosition += glm::normalize(movementDir) * 0.05f * multiplier;
	}

}

glm::mat4 rotationMatrix(1);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
	glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GL_TRUE);
	glfwSetErrorCallback(error_callback);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Raytracing In OpenGL", NULL, NULL);
	logger::Log(logger::LogLevel::INFO, std::string("GL window created with OpenGL version ") + std::to_string(OPENGL_MAJOR_VERSION) + "." + std::to_string(OPENGL_MINOR_VERSION));

	if(!window)
	{
		logger::Log(logger::LogLevel::FATAL, std::string("Failed to create GL window"));
		ForceTerminate();
		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);

	// vsync
	if(vSync){
		glfwSwapInterval(1);
		logger::Log(logger::LogLevel::INFO, std::string("V-Sync enabled"));
	}
	else {
		glfwSwapInterval(0);
		logger::Log(logger::LogLevel::INFO, std::string("V-Sync disabled"));
	}

	// loading glad
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		logger::Log(logger::LogLevel::FATAL, std::string("Failed to initialize GLAD"));
		ForceTerminate();
		return EXIT_FAILURE;
	}

	logger::Log(logger::LogLevel::INFO, std::string("GLAD initialized"));
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);


	GLuint vertexShader = loadShader("shaders/trivertex.vert.glsl", GL_VERTEX_SHADER);
	GLuint fragmentShader = loadShader("shaders/trifragment.frag.glsl", GL_FRAGMENT_SHADER);

	GLuint shaderProg = createShaderProgram(std::vector({vertexShader, fragmentShader}));


	DeleteGLItem(vertexShader);
	DeleteGLItem(fragmentShader);
	
	GLuint VAO, VBO, EBO;
	std::vector<GLuint> objects = alltheobjects();
	VAO = objects[0];
	VBO = objects[1];
	EBO = objects[2];

	GLuint uvBuffer;
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(UVs), UVs, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(VAO);


	glUseProgram(shaderProg);

	placeBasicScene();
	bindAll(shaderProg);

	glUniform1f(glGetUniformLocation(shaderProg, "aspectRatio"), (float)SCR_WIDTH / (float)SCR_HEIGHT);
	

	while(!glfwWindowShouldClose(window))
	{
		input_callback(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(VAO);
		rotationMatrix = glm::rotate(glm::rotate(glm::mat4(1), cameraPitch, glm::vec3(1, 0, 0)), cameraYaw, glm::vec3(0, 1, 0));
		handleMovement(window, cameraPosition, cameraYaw, cameraPitch, &rotationMatrix);
		// send the rotation matrix to the shader along with the camera position
		glUniformMatrix4fv(glGetUniformLocation(shaderProg, "rotationMatrix"), 1, GL_FALSE, glm::value_ptr(rotationMatrix));
		glUniform3f(glGetUniformLocation(shaderProg, "cameraPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	DeleteGLItem(VAO);
	DeleteGLItem(VBO);
	DeleteGLItem(EBO);
	DeleteGLItem(shaderProg);
	glfwTerminate();
	logger::Log(logger::LogLevel::INFO, std::string("Terminating"));
	return EXIT_SUCCESS;
}
