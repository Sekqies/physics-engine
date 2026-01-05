#version 330

out vec4 FragColor;

in vec3 shared_pos;
void main(){
	FragColor = vec4(abs(shared_pos)*0.05,1.0f);
}
