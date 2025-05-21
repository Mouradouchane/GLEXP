#pragma once

#ifndef TEXTUER_2D_CPP
#define TEXTUER_2D_CPP

#include "texture_2d.hpp"

/*
	constructor's
*/
texture_2d::texture_2d(
	void* data, 
	uint16_t width, 
	uint16_t height,
	GLint data_type,     
	GLint data_format,          
	GLint mip_map_levels, 
	GLint wrap_style_x,
	GLint wrap_style_y,
	GLint min_filtering_style,
	GLint mag_filtering_style 
) {
	if (data == nullptr) return;

	// generate texture
	glGenTextures(1, &this->id);
	
	// bind texture 
	glBindTexture(GL_TEXTURE_2D, this->id);
	
	// set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_style_x);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_style_y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filtering_style);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filtering_style);
	
	// copy texture data to gpu memory
	glTexImage2D(
		GL_TEXTURE_2D,
		mip_map_levels,
		GL_RGB,
		width,
		height,
		0,
		data_format,
		GL_UNSIGNED_BYTE,
		data
	);

	// generate mip_maps
	glGenerateMipmap(mip_map_levels);

	// unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

}

texture_2d::texture_2d(
	const image* _image , 
	GLint data_type        , 
	GLint data_format      ,
	GLint mip_map_levels   ,
	GLint wrap_style_x     ,
	GLint wrap_style_y     ,
	GLint min_filtering_style,
	GLint mag_filtering_style
) {
	GLenum  glerr = GL_NO_ERROR; 
	uint8_t const* data = _image->buffer();

	if (data == nullptr) return;

	// generate texture
	glGenTextures(1, &this->id);
	glerr = glGetError();

	// bind texture 
	glBindTexture(GL_TEXTURE_2D, this->id);
	glerr = glGetError();

	// set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_style_x);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_style_y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filtering_style);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filtering_style);

	// copy texture data to gpu memory
	glTexImage2D(
		GL_TEXTURE_2D,
		mip_map_levels,
		GL_RGB,
		GLsizei(_image->get_width()),
		GLsizei(_image->get_height()),
		0,
		data_format,
		GL_UNSIGNED_BYTE,
		data
	);
	glerr = glGetError();

	// generate mip_maps
	//glGenerateMipmap(mip_map_levels);
	glerr = glGetError();

	// unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

}

/*
	destructor
*/ 
texture_2d::~texture_2d() {

	if (glIsTexture(this->id)) {
		glDeleteTextures(1, &this->id);
	}
}

/*
	class functions
*/

void texture_2d::bind() {
	glBindTexture(GL_TEXTURE_2D, this->id);
}

void texture_2d::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

/*
	class static functions
*/

void texture_2d::set_parameter_i(
	GLuint textuer_id, GLenum name, GLint value
) {
	glBindTexture(GL_TEXTURE_2D, textuer_id);
	glTexParameteri(GL_TEXTURE_2D, name, value);
	glBindTexture(GL_TEXTURE_2D, 0);
}

#endif
