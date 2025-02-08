#pragma once

#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "assert.hpp"
#include "errors.hpp"

#include "glew/glew.h"
#include "glfw/glfw3.h"
#include "texture_2d.hpp"

#define MSG_SIZE 1024
#define MIPMAP_ZERO_LEVEL 0

extern GLuint EBO;
extern GLuint VBO;
extern GLuint VAO;

struct texture_parameters {
	GLint data_type;
	GLint channel;
	GLint mip_map_levels;
	GLint wrap_style_x;
	GLint wrap_style_y;
	GLint filter_min_style;
	GLint filter_mag_style;
};

struct vbo {
	GLuint id;

	vbo();
	~vbo();

	void bind();
	void unbind();
};
struct ebo : vbo {

};
struct vao {
	GLuint id = NULL; // vao id
	std::vector<vbo> vbos; 
	std::vector<GLuint> ebos;

	vao();
	~vao();
	void bind();
	void unbind();
};

std::string* load_shader_source_code(std::string const& shader_file_path);

struct shader_object{
	ERR last_error = ERR::NO_ERR;
	GLuint  type = NULL;
	GLuint  id   = NULL;
};

class shader {

public:
	ERR last_error = ERR::NO_ERR;
	GLuint  id = NULL; // program shader id

	shader(std::string const& shader_code, std::string const& fragement_shader_path);
	~shader();

	void use();

	GLint get_uniform_location(std::string const& uniform_name);

	void   set_int(GLint uniform_location, int value);
	void  set_bool(GLint uniform_location, bool value);
	void set_float(GLint uniform_location, float value);
};

void init_data_for_shader();

ERR init_textures();

#endif
