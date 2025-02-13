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

// std::string* load_shader_source_code(std::string const& shader_file_path);

// used to represent "vertex shader","fragement shader",...
struct shader_object{
	ERR last_error = ERR::NO_ERR;
	GLuint type = NULL;
	GLuint id   = NULL;
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
