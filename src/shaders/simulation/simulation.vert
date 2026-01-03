#version 330

layout (location=0) in vec4 pos;

uniform mat4 projection, view, model;

void main(){
	gl_Position = projection * view * model * pos;
}
