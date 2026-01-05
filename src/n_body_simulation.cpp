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
#include <texture/texture.h>


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

constexpr Point null_point{0.0f,vec3(0.0f,0.0f,0.0f),vec3(0.0f,0.0f,0.0f),vec3(0.0f,0.0f,0.0f)};



class RigidBody {	
public:
	float density,volume;
	float theta, phi;
	Point center_of_mass;
	mat3 inertia_tensor;
	vector<vec3> vertices;
	RigidBody(const float density, const vector<vec3>& vertices, const float theta=0.0f, const float phi = 0.0f) {
		this->density = density;
		this->vertices = vertices;
		this->theta = theta;
		this->phi = phi;
		this->center_of_mass = null_point;
		this->inertia_tensor = mat3(0.0f);
		this->compute_center_of_mass();
		this->compute_inertia_tensor();
	}
private:
	void compute_center_of_mass() {
		float total_volume = 0.0f;
		float signed_volume = 0.0f;
		vec3 centroid = vec3(0.0f);
		const unsigned int n = vertices.size();
		const vec3 origin = vertices[0];
		for (unsigned int i = 0; i < n; i+=3) {
			const vec3 a = vertices[i];
			const vec3 b = vertices[i + 1];
			const vec3 c = vertices[i + 2];
			signed_volume = 0.16666f * glm::determinant(mat3(a-origin, b-origin, c-origin));
			centroid = (origin + a + b + c) * 0.25f;
			total_volume += signed_volume;
			center_of_mass.position += centroid * signed_volume;
		}
		volume = total_volume;
		center_of_mass.position /= total_volume;
		center_of_mass.mass = total_volume * density;
	}
	void compute_inertia_tensor(){
		using glm::outerProduct;
		mat3 covariance_matrix = mat3(0.0f);
		for (unsigned int i = 0; i < vertices.size(); i+=3) {
			const vec3 a = vertices[i];
			const vec3 b = vertices[i + 1];
			const vec3 c = vertices[i + 2];
			const vec3 sum = a + b + c;
			const float signed_volume = glm::determinant(mat3(a, b, c)) / 120.0f;
			covariance_matrix += signed_volume * (outerProduct(a, a) + outerProduct(b, b) + outerProduct(c, c) + outerProduct(sum, sum));
		}
		const float trace = covariance_matrix[0][0] + covariance_matrix[1][1] + covariance_matrix[2][2];
		const mat3 inertia_at_origin = density * (trace * mat3(1.0f) - covariance_matrix);
		const vec3 r = center_of_mass.position;
		const float r2 = glm::dot(r, r);
		const mat3 shift = (mat3(r2) - outerProduct(r, r)) * center_of_mass.mass;
		inertia_tensor = inertia_at_origin - shift;
	}
};






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
	glPointSize(15.0f);
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