#include <iostream>
#include <utility>
#include <memory>
#include <tuple>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GLFW/glfw3.h>
#include <shader/shader.h>
#include <VAO/VAO.h>
#include <VBO/VBO.h>
#include <texture/texture.h>


using std::cout, std::cerr, std::cin, std::string, std::vector, std::unique_ptr;
using glm::mat4, glm::mat3, glm::mat2, glm::vec4, glm::vec3, glm::quat;

constexpr float viewport_width = 800.00f;
constexpr float viewport_height = 600.00f;


typedef struct Point {
	float mass;
	vec3 position;
	vec3 linear_momentum;
	vec3 force;
} Point;

constexpr Point null_point{ 0.0f,vec3(0.0f,0.0f,0.0f),vec3(0.0f,0.0f,0.0f),vec3(0.0f,0.0f,0.0f) };

struct RenderObject {
	unique_ptr<VAO> vao;
	unique_ptr<VBO> vbo;
};
vector<RenderObject> renderables;

class RigidBody {
public:
	// State variables
	Point center_of_mass;
	vec3 angular_momentum, torque;
	quat orientation_quat;

	// Constants
	float density, volume;
	mat3 inertia_tensor, inverse_inertia_tensor;

	//Auxiliary variables
	vec3 velocity, angular_velocity;
	mat3 world_inertia, inverse_world_inertia, rotation_matrix;

	vector<vec3> vertices;
	RigidBody(const float density, const vector<vec3>& vertices,
		const quat orientation_quat = quat(1.0f, 0.0f, 0.0f, 0.0f),
		const vec3 linear_momentum = vec3(0.0f, 0.0f, 0.0f),
		const vec3 angular_momentum = vec3(0.0f, 0.0f, 0.0f)) {
		this->density = density;
		this->vertices = vertices;

		this->center_of_mass = null_point;
		this->center_of_mass.linear_momentum = linear_momentum;
		this->inertia_tensor = mat3(0.0f);

		this->angular_momentum = angular_momentum;
		this->orientation_quat = orientation_quat;

		this->compute_center_of_mass();
		this->compute_inertia_tensor();

		// initialize auxiliary variables to 0
		angular_velocity = vec3(0.0f);
		velocity = vec3(0.0f);
		this->center_of_mass.force = vec3(0.0f);
		torque = vec3(0.0f);
	}
	/// <summary>
	/// Updates the state of the rigid body. Variables considered as "state" are
	/// - center_of_mass.position (x)
	/// - center_of_mass.linear_momentum (p)
	/// - angular_momentum (P)
	/// - orientation_quat (q)
	/// </summary>
	/// <param name="delta_time">The time dt in which the simulation happens</param>
	void update_state(const float delta_time) {
		angular_momentum += torque * delta_time;
		center_of_mass.linear_momentum += center_of_mass.force * delta_time;

		center_of_mass.position += velocity * delta_time;
		const quat spin_quat = quat(0.0f, angular_velocity.x, angular_velocity.y, angular_velocity.z);
		orientation_quat += 0.5f * (spin_quat * orientation_quat) * delta_time;
		orientation_quat = glm::normalize(orientation_quat);

		center_of_mass.force = vec3(0.0f);
		torque = vec3(0.0f);
	}
	/// <summary>
	/// Updates auxiliary variables used for calculations. The auxiliary variables are:
	/// - velocity (v)
	/// - rotation_matrix (R)
	/// - world_inertia (I)
	/// - inverse world_inertia (I^-1)
	/// - angular_velocity (omega)
	/// </summary>
	void update_auxiliary_variables() {
		velocity = center_of_mass.linear_momentum / center_of_mass.mass;
		rotation_matrix = glm::mat3_cast(orientation_quat);
		inverse_world_inertia = rotation_matrix * inverse_inertia_tensor * glm::transpose(rotation_matrix);
		world_inertia = rotation_matrix * inertia_tensor * glm::transpose(rotation_matrix);
		angular_velocity = inverse_world_inertia * angular_momentum;
	}
private:
	/// <summary>
	/// Determines que center of mass in local coordinates.
	/// </summary>
/// <summary>
	/// Determines que center of mass in local coordinates and shifts vertices to match
	/// </summary>
	void compute_center_of_mass() {
		float total_volume = 0.0f;
		float signed_volume = 0.0f;
		vec3 centroid = vec3(0.0f);
		vec3 com_accumulator = vec3(0.0f);
		const unsigned int n = vertices.size();
		const vec3 origin = vertices[0];

		for (unsigned int i = 0; i < n; i += 3) {
			const vec3 a = vertices[i];
			const vec3 b = vertices[i + 1];
			const vec3 c = vertices[i + 2];
			signed_volume = 0.16666f * glm::determinant(mat3(a - origin, b - origin, c - origin));
			centroid = (origin + a + b + c) * 0.25f;
			total_volume += signed_volume;
			com_accumulator += centroid * signed_volume;
		}

		this->volume = total_volume;
		this->center_of_mass.mass = total_volume * density;

		vec3 com_offset = com_accumulator / total_volume;
		this->center_of_mass.position = com_offset;

		for (auto& v : this->vertices) {
			v -= com_offset;
		}
	}
	/// <summary>
	/// Computes the objects's local inertia tensor (I_body) and inverse inertia tensor (I_body^-1)
	/// </summary>
	void compute_inertia_tensor() {
		using glm::outerProduct;
		mat3 covariance_matrix = mat3(0.0f);
		for (unsigned int i = 0; i < vertices.size(); i += 3) {
			const vec3 a = vertices[i];
			const vec3 b = vertices[i + 1];
			const vec3 c = vertices[i + 2];
			const vec3 sum = a + b + c;
			const float signed_volume = glm::determinant(mat3(a, b, c)) / 120.0f;
			covariance_matrix += signed_volume * (outerProduct(a, a) + outerProduct(b, b) + outerProduct(c, c) + outerProduct(sum, sum));
		}
		const float trace = covariance_matrix[0][0] + covariance_matrix[1][1] + covariance_matrix[2][2];
		const mat3 inertia_at_origin = density * (trace * mat3(1.0f) - covariance_matrix);

		this->inertia_tensor = inertia_at_origin;
		this->inverse_inertia_tensor = glm::inverse(inertia_tensor);
	}
};
constexpr float EPSILON = 0.005f;

vec3 gravity_torque(const RigidBody& rb, const vec3& r, const float mu) {
	float dist2 = glm::dot(r, r);
	if (dist2 < EPSILON) dist2 = EPSILON;
	const float inv_r = glm::inversesqrt(dist2);
	const float inv_r2 = inv_r * inv_r;
	return 3.0f * mu * inv_r2 * inv_r2 * inv_r * glm::cross(r, rb.world_inertia * r);
}

vec3 gravity_force(const RigidBody& rb, const vec3& r, const float mu) {
	float dist2 = glm::dot(r, r);
	if (dist2 < EPSILON) dist2 = EPSILON;
	return rb.center_of_mass.mass * mu / dist2 * glm::normalize(r);
}




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
	vector<RigidBody> bodies;
	vector<std::tuple<vec3, vec3, quat, vec3>> starting_conditions;
	vector<vec3> tetrahedron_verts = {
		{0.0f,  1.0f,  0.0f}, {-1.0f, -1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f},
		{0.0f,  1.0f,  0.0f}, { 1.0f, -1.0f,  1.0f}, { 0.0f, -1.0f, -1.0f},
		{0.0f,  1.0f,  0.0f}, { 0.0f, -1.0f, -1.0f}, {-1.0f, -1.0f,  1.0f},
		{-1.0f, -1.0f,  1.0f}, { 0.0f, -1.0f, -1.0f}, { 1.0f, -1.0f,  1.0f}
	};

	bodies.emplace_back(1.0f, tetrahedron_verts);
	bodies.emplace_back(1.0f, tetrahedron_verts);


	starting_conditions.emplace_back(
		vec3(4.0f, 0.0f, -1.0f),
		vec3(0.0f, 0.4f, 0.0f),
		quat(1.0f, 0.0f, 0.0f, 0.0f),
		vec3(0.1f, 0.0f, 0.0f)
	);
	starting_conditions.emplace_back(
		vec3(5.0f, 0.0f, 0.0f),
		vec3(0.0f, -0.4f, 0.0f),
		quat(1.0f, 0.0f, 0.0f, 0.0f),
		vec3(0.2f, 0.0f, 0.0f)
	);
	for (unsigned int i = 0; i < bodies.size(); i++) {
		auto [position, velocity, rotation_quat, angular_velocity] = starting_conditions[i];
		bodies[i].center_of_mass.position += position;
		bodies[i].center_of_mass.linear_momentum += velocity * bodies[i].center_of_mass.mass;
		bodies[i].orientation_quat = rotation_quat;
		bodies[i].update_auxiliary_variables();
		bodies[i].angular_momentum = bodies[i].world_inertia * angular_velocity;
	}
	for (const auto& body : bodies) {
		RenderObject obj;
		vector<float> raw;
		for (const auto& v : body.vertices) {
			raw.push_back(v.x); raw.push_back(v.y); raw.push_back(v.z);
		}
		obj.vbo = std::make_unique<VBO>(raw);
		obj.vao = std::make_unique<VAO>();
		obj.vao->bind();
		obj.vao->set_attributes(*obj.vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
		obj.vao->unbind();
		renderables.push_back(std::move(obj));
	}

	constexpr float delta_time = 0.005f;

	mat4 projection = glm::perspective(glm::radians(45.0f), (float)viewport_width / (float)viewport_height, 0.1f, 200.0f);
	mat4 view = mat4(1.0f);
	view = glm::translate(view, vec3(0.0f, 0.0f, -30.0f));
	glPointSize(15.0f);
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.3f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader_program.use();
		vao.bind();
		for (unsigned int i = 0; i < bodies.size(); ++i) {
			bodies[i].update_auxiliary_variables();
			for (unsigned int j = 0; j < bodies.size(); ++j) {
				if (i == j) continue;
				const vec3 r = bodies[j].center_of_mass.position - bodies[i].center_of_mass.position;
				const float mu = bodies[j].center_of_mass.mass * G;
				const vec3 F = gravity_force(bodies[i], r, mu);
				const vec3 torque = gravity_torque(bodies[i], r, mu);
				bodies[i].center_of_mass.force += F;
				bodies[i].torque += torque;
			}
		}
		for (unsigned int i = 0; i < bodies.size(); ++i) {
			bodies[i].update_state(delta_time);
			mat4 model = mat4(1.0f);
			model = glm::translate(model, bodies[i].center_of_mass.position);
			model *= glm::mat4_cast(bodies[i].orientation_quat);
			const vec3 com = bodies[i].center_of_mass.position;
			const quat q = bodies[i].orientation_quat;
			shader_program.setMat4("model", model);
			shader_program.setMat4("view", view);
			shader_program.setMat4("projection", projection);
			renderables[i].vao->bind();
			glDrawArrays(GL_TRIANGLES, 0, bodies[i].vertices.size());
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}