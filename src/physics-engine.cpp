/*
#include <glad/glad.h>
#include <shader/shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <iostream>

using std::cout, std::cin, std::string;

constexpr float viewport_height = 1200.00f;
constexpr float viewport_width = 1600.00f;


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
void change_value(GLFWwindow* window, float& magnitude) {
	const bool is_up = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
	const bool is_down = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
	if (!is_up && !is_down) 
		return;
	int sign = 1;
	if (!is_up)
		sign = -1;
	constexpr float increase = 0.0005f;
	magnitude += increase * sign;
	if (magnitude > 1.0f) {
		magnitude = 1.0f;
	}
	if (magnitude < 0.0f) {
		magnitude = 0.0f;
	}
}

int has_compilation_error(const unsigned int shader) {
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	return success == 0;
}

void see_compilation_result(const unsigned int shader) {
	char infoLog[512];
	if (has_compilation_error(shader)) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		cout << "Error: " << infoLog;
	}
}


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(viewport_width, viewport_height, "OpenGL Window", nullptr, nullptr);
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
	// Coordinates in OpenGL are to be inputted in the range (-1,1). 
	// This is essentially telling us to map (-1,1) to (0,800) and (0,600) for each coordinate
	glViewport(0, 0, viewport_width, viewport_height);

	Shader shader_program("shaders/vertex/vertex.glsl", "shaders/fragment/fragment.glsl");

	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	float tex_coords[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.5f, 1.0f
	};
	int width, height, nr_channels;
	unsigned char* data = stbi_load("assets/container.jpg", &width, &height, &nr_channels, 0);
	if (!data) {
		cout << "Error generating textures from image";
	}
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	int w, h, n;
	unsigned char* data_2 = stbi_load("assets/awesomeface.png", &w, &h, &n, 0);
	if (!data_2) {
		cout << "Error reading image awesomeface.jpg";
	}
	unsigned int texture_2;
	glGenTextures(1, &texture_2);
	glBindTexture(GL_TEXTURE_2D, texture_2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_2);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);


	unsigned int VAO, VBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	const unsigned int stride = 5 * sizeof(float);
	// (x,y,z)

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	// (textures)
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glEnable(GL_DEPTH_TEST);

	const float offset = 0.25f;
	shader_program.use();
	shader_program.setInt("texture_image_1", 0);
	shader_program.setInt("texture_image_2", 1);

	glm::vec3 cube_positions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


		
	float mix_level = 0.2f;
	glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 camera_target = glm::vec3(1.0f, 0.5f, -3.0f);
	glm::vec3 camera_direction = glm::normalize(camera_position - camera_target);
	
	// This is our rendering loop.
	while (!glfwWindowShouldClose(window)) {
		processInput(window, GLFW_KEY_ENTER);
		glClearColor(0.3f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		const float time = glfwGetTime();
		
		shader_program.use();
		shader_program.setFloat("offset", offset);
		change_value(window, mix_level);
		shader_program.setFloat("mix_level", mix_level);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, texture_2);

		glBindVertexArray(VAO);
		for (unsigned int i = 0; i < 10; ++i) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cube_positions[i]);

			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
			glm::mat4 view = glm::mat4(1.0f);
			view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));

			glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)viewport_height / (float)viewport_height, 0.1f, 100.0f);

			shader_program.setMat4("projection", projection);
			shader_program.setMat4("view", view);
			shader_program.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// --------- END OF RENDERING PROCESS ---------
		// This switches the current color buffer with a new one. Presumably,
		// something should happen during the rendering loop. 
		glfwSwapBuffers(window);
		// This catches events, such as the window resizing callback.
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}*/