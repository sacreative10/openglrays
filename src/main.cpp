#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "util/GLutil.h"
#include "util/logger.h"


const int OPENGL_MAJOR_VERSION = 4;
const int OPENGL_MINOR_VERSION = 6;

const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

GLfloat Vertices[] =
{
	0.5f,  0.5f, 0.0f,  // top right 
	0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f, -0.5f, 0.0f,  // bottom left
	-0.5f,  0.5f, 0.0f
};

GLuint Indices[] =
{
	0, 1, 3,
	1, 2, 3
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


	GLuint vertexShader = loadShader("shaders/trivertex.glsl", GL_VERTEX_SHADER);
	GLuint fragmentShader = loadShader("shaders/trifragment.glsl", GL_FRAGMENT_SHADER);

	GLuint shaderProg = createShaderProgram(std::vector({vertexShader, fragmentShader}));


	DeleteGLItem(vertexShader);
	DeleteGLItem(fragmentShader);
	
	GLuint VAO, VBO, EBO;
	std::vector<GLuint> objects = alltheobjects();
	VAO = objects[0];
	VBO = objects[1];
	EBO = objects[2];


	while(!glfwWindowShouldClose(window))
	{
		input_callback(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProg);
		glBindVertexArray(VAO);
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
