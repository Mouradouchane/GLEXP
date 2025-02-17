#pragma once

#ifndef TEXTUER_2D_HPP
#define TEXTUER_2D_HPP

#include <string>
#include "glew/glew.h"
#include "glfw/glfw3.h"

#include "image.hpp"

enum class TEXTURE_TYPE : uint16_t {
	DEFAULT = 0,
	DIFFUSE,
	SPECULAR,
};

struct texture_parameters {
	GLint data_type;
	GLint channel;
	GLint mip_map_levels;
	GLint wrap_style_x;
	GLint wrap_style_y;
	GLint filter_min_style;
	GLint filter_mag_style;
};

class texture_2d {
public:
	GLuint id = NULL;
	TEXTURE_TYPE type = TEXTURE_TYPE::DEFAULT;

	// constructor's
	texture_2d() = default;
	texture_2d(
		void* data, 
		uint16_t width, 
		uint16_t height,
		GLint data_type        = GL_UNSIGNED_BYTE, 
		GLint data_format      = GL_RGB,
		GLint mip_map_levels   = 0,
		GLint wrap_style_x     = GL_REPEAT,
		GLint wrap_style_y     = GL_REPEAT,
		GLint min_filtering_style = GL_LINEAR,
		GLint mag_filtering_style = GL_LINEAR
	);
	texture_2d(
		const image* _image, 
		GLint data_type        = GL_UNSIGNED_BYTE, 
		GLint data_format      = GL_RGB,
		GLint mip_map_levels   = 0,
		GLint wrap_style_x     = GL_REPEAT,
		GLint wrap_style_y     = GL_REPEAT,
		GLint min_filtering_style = GL_LINEAR,
		GLint mag_filtering_style = GL_LINEAR
	);

	~texture_2d();

	void bind();
	void unbind();

	static void set_parameter_i(
		GLuint textuer_id, GLenum name, GLint value
	);

};

#endif
