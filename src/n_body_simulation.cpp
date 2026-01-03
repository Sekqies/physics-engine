#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <shader/shader.h>
#include <VAO/VAO.h>
#include <VBO/VBO.h>


using std::cout, std::cerr, std::cin, std::string, std::vector, std::unique_ptr;
using glm::mat4, glm::mat3, glm::mat2, glm::vec4, glm::vec3;

constexpr float viewport_width = 800.00f;
constexpr float viewport_height = 600.00f;

typedef struct Point {
	float mass;
	vec3 position;
	vec3 velocity;
	vec3 acceleration;
} Point;




GLFWwindow* initalize_window(const float width, const float height, const string windowname) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(width, height, windowname.c_str(), nullptr, nullptr);
	if (window == nullptr) {
		cerr << "Error at initialize_window: error at creating window";
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	const int worked = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (!worked) {
		cerr << "Error at initialize_window: error at initializing GLAD";
		return nullptr;
	}
	glViewport(0, 0, viewport_width, viewport_height);
	return window;
}


int main()
{
	GLFWwindow* window = initalize_window(viewport_width, viewport_height, "Orbit Simulation");
	Shader shader_program("shaders/simulation/simulation.vert", "shaders/simulation/simulation.frag");
	const vector<float> point{ 0.0f,0.0f,0.0f };
	VBO vertex_vbo(point);
	VAO vao;
	vao.set_attributes(vertex_vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	vao.unbind();

	// setting up transformation matrices
	constexpr float G = 1.0f;

	vector<Point> points = {
		{ 1000.0f, vec3(0.0f, 0.0f, 0.0f),   vec3(0.0f, 0.0f, 0.0f),   vec3(0.0f) },
		{ 10.0f,   vec3(6.0f, 0.0f, 0.0f),   vec3(0.0f, 12.0f, 0.0f),  vec3(0.0f) },
		{ 0.1f,    vec3(0.0f, -8.0f, 0.0f),  vec3(-9.0f, 0.0f, 0.0f),  vec3(0.0f) },
	};
	const unsigned int n = points.size();

	mat4 view = mat4(1.0f);
	view = glm::translate(view, vec3(0.0f, 0.0f, -50.0f));
	mat4 projection = glm::perspective(glm::radians(45.0f), (float)viewport_width / (float)viewport_height, 0.1f, 100.0f);
	const float delta_time = 0.001f;
	glPointSize(10.0f);
	while (!glfwWindowShouldClose(window)){
		glClearColor(0.3f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader_program.use();
		vao.bind();
		for (unsigned int i = 0; i < n; ++i) {
			points[i].position += points[i].velocity * delta_time;
			points[i].acceleration = vec3(0.0f);
		}
		for (unsigned int i = 0; i < n; ++i) {
			for (unsigned int j = 0; j < n; ++j) {
				if (i == j) continue;
				const vec3 direction = points[j].position - points[i].position;
				const float distance = glm::length(direction);
				points[i].acceleration += G * points[j].mass / (distance*distance) * glm::normalize(direction);
			}
			points[i].velocity += points[i].acceleration * delta_time;
			mat4 model = mat4(1.0f);
			model = glm::translate(model, points[i].position);
			shader_program.setMat4("model", model);
			shader_program.setMat4("projection", projection);
			shader_program.setMat4("view", view);
			glDrawArrays(GL_POINTS, 0, 1);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}