#version 330

out vec4 FragColor;

in vec3 shared_pos;
void main(){
	//FragColor = vec4(0.0f,1.0f,0.0f,1.0f);
	FragColor = vec4(abs(shared_pos)*0.10,1.0f);
}
