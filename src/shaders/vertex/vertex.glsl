#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 a_tex_coord;

out vec2 tex_coord;
uniform mat4 projection, view, model;

void main() {
	tex_coord = a_tex_coord;
	gl_Position = projection * view * model * vec4(pos, 1.0);

}	
