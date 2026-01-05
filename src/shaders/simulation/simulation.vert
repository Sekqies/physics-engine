#version 330

layout (location=0) in vec4 pos;

uniform mat4 projection, view, model;

out vec3 shared_pos;

void main(){
	vec4 world_position = model * pos;
	shared_pos = vec3(world_position);
	gl_Position = projection * view * model * pos;
}
