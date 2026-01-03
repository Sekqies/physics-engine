#pragma once
#include <string>
#include <iostream>
#include <stb_image/stb_image.h>
#include <shader/shader.h>
#include <glad/glad.h>

using std::string;

class Texture {
public:
	unsigned int ID;
	unsigned int texture_code;
	GLenum format;
	GLenum target;
	/// <summary>
	/// Initializes an OpenGL texture with an image
	/// </summary>
	/// <param name="image_path">The path to the image whose data will be used to make the texture</param>
	/// <param name="format">The format of byte encoding in the image. Defaults to GL_RGB</param>
	/// <param name="target">The internal OpenGL target (defaults to GL_TEXTURE_2D)</param>
	Texture(const string& image_path, const GLenum format = GL_RGB, const GLenum target = GL_TEXTURE_2D, const GLenum min_filter = GL_LINEAR_MIPMAP_NEAREST, const GLenum mag_filter = GL_LINEAR, const GLenum texture_wrap = GL_REPEAT) {
		this->format = format;
		this->target = target;
		glGenTextures(1, &ID);
		glBindTexture(target, ID);
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(image_path.c_str(), &width, &height, &nr_channels, 0);
		if (!data) {
			std::cerr << "Error when loading Texture: failed at loading image " << image_path;
		}
		glTexImage2D(target, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(target);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture_wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture_wrap);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
;		stbi_image_free(data);
	}
	void attach_to_program(const Shader& shader_program, const string& name, const int internal_code) {
		shader_program.setInt(name, internal_code);
		texture_code = internal_code;
	}
	void activate() {
		glActiveTexture(GL_TEXTURE0 + texture_code);
		bind();
	}
	void bind() {
		glBindTexture(target, ID);
	}
private:
	int width;
	int height;
	int nr_channels;
};