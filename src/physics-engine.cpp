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
	FragColor = vec4(1.0f,0.5f,0.25f,0.5f);
}
)";

const char* fragmentShaderSource2 = R"(
#version 330 core
out vec4 FragColor;

void main(){
	FragColor = vec4(0.5f,1.0f,0.25f,0.5f);
}
)";

const char* fragmentShaderSources[] = { fragmentShaderSource, fragmentShaderSource2 };
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


	unsigned int fragmentShaders[2];
	for (unsigned int i = 0; i < 2; ++i) {
		fragmentShaders[i] = glCreateShader(GL_FRAGMENT_SHADER);
		cout << &fragmentShaderSources[i];
		glShaderSource(fragmentShaders[i], 1, &fragmentShaderSources[i],NULL);
		glCompileShader(fragmentShaders[i]);
		see_compilation_result(fragmentShaders[i]);
	}
	unsigned int shaderPrograms[2];
	for (unsigned int i = 0; i < 2; ++i) {
		shaderPrograms[i] = glCreateProgram();
		glAttachShader(shaderPrograms[i], vertexShader);
		glAttachShader(shaderPrograms[i], fragmentShaders[i]);
		glLinkProgram(shaderPrograms[i]);
	}

	glDeleteShader(vertexShader);
	for (unsigned int i = 0; i < 2; ++i) {
		glDeleteShader(fragmentShaders[i]);
	}
	float vertices[9] = {
	-0.5f,-0.5f,0.0f,
	0.5f,-0.5f,0.0f,
	0.0f,0.5f,0.0f,
	};

	float vertices2[9] = {
	0.5f,0.5f,0.0f,
	-0.5f,0.5f,0.0f,
	0.0f,-0.5f,0.0f,
	};

	float vertices_array[2][9] = {
	{-0.5f,-0.5f,0.0f,
	0.5f,-0.5f,0.0f,
	0.0f,0.5f,0.0f},
	{0.5f,0.5f,0.0f,
	-0.5f,0.5f,0.0f,
	0.0f,-0.5f,0.0f}
	};

	unsigned int VAO_array[2];
	unsigned int VBO_array[2];
	glGenVertexArrays(2, VAO_array);
	glGenBuffers(2, VBO_array);
	for (unsigned int i = 0; i < 2; ++i) {
		glBindVertexArray(VAO_array[i]);
		glBindBuffer(GL_ARRAY_BUFFER,VBO_array[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_array[i]), vertices_array[i], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}
	// This is our rendering loop.
	while (!glfwWindowShouldClose(window)) {
		processInput(window, GLFW_KEY_ENTER);
		glClearColor(0.3f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		for (unsigned int i = 0; i < 2; ++i) {
			glUseProgram(shaderPrograms[i]);
			glBindVertexArray(VAO_array[i]);
			glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices_array[i]) / sizeof(float));
		}

		// --------- END OF RENDERING PROCESS ---------
		// This switches the current color buffer with a new one. Presumably,
		// something should happen during the rendering loop. 
		glfwSwapBuffers(window);
		// This catches events, such as the window resizing callback.
		glfwPollEvents();
	}
	glDeleteVertexArrays(2, VAO_array);
	glDeleteBuffers(2, VBO_array);
	for (unsigned int i = 0; i < 2; i++) {
		glDeleteProgram(shaderPrograms[i]);
	}
	glfwTerminate();
	return 0;
}
