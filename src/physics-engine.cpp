#include "physics-engine.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using std::cout, std::cin, std::string;

unsigned int add(const unsigned int a, const unsigned int b) {
	return a + b;
}

// This function is called every time the window width and height is changed.
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, const int key) {
	if (glfwGetKey(window, key) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int has_compilation_error(const unsigned int shader) {
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	return success==0;
}

void see_compilation_result(const unsigned int shader) {
	char infoLog[512];
	if (has_compilation_error(shader)) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		cout << "Error: " << infoLog;
	}
}

const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 pos;

void main() {
	gl_Position = vec4(pos, 1.0);
}	
)";
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main(){
	FragColor = vec4(1.0f,0.5f,0.25f,1.0f);
}
)";

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Window", nullptr, nullptr);
	if (window == nullptr) {
		cout << "Error initializing the window";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	const int worked = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (!worked) {
		cout << "Error initializing GLAD";
		return -1;
	}
	// Coordinated in OpenGL are to be inputted in the range (-1,1). 
	// This is essentially telling us to map (-1,1) to (0,800) and (0,600) for each coordinate
	glViewport(0, 0, 800, 600);

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	see_compilation_result(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	see_compilation_result(fragmentShader);

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	float vertices[] = {
	-0.5f,-0.5f,0.0f,
	0.5f,-0.5f,0.0f,
	0.0f,0.5f,0.0f,
	};


	unsigned int VAO,VBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(0);

	// This is our rendering loop.
	while (!glfwWindowShouldClose(window)) {
		processInput(window, GLFW_KEY_ENTER);
		glClearColor(0.3f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices)/sizeof(float));

		// --------- END OF RENDERING PROCESS ---------
		// This switches the current color buffer with a new one. Presumably,
		// something should happen during the rendering loop. 
		glfwSwapBuffers(window);
		// This catches events, such as the window resizing callback.
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
	glfwTerminate();
	return 0;
}
