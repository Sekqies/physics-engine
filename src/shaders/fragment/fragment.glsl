#version 330 core
out vec4 FragColor;
in vec2 tex_coord;

uniform sampler2D texture_image_1;
uniform sampler2D texture_image_2;
uniform float mix_level;

void main(){
	vec4 t1 = texture(texture_image_1,tex_coord);
	vec4 t2 = texture(texture_image_2,tex_coord);
	FragColor = mix(t1,t2,mix_level);						
}