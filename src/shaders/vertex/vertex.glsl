#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 a_color;
layout(location = 2) in vec2 a_tex_coord;

out vec3 shared_color;
out vec2 tex_coord;
uniform float offset;

void main() {
	shared_color = a_color;
	tex_coord = a_tex_coord;
	gl_Position = vec4(pos.x + offset,-pos.y,pos.z, 1.0);

}	
