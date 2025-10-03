#pragma once

#ifndef SHADER_CPP
#define SHADER_CPP

/*
	todo: rework
*/
#if 0

#include <string>
#include <vector>
#include <fstream>
#include <sstream>


#include "core/errors/assert.hpp"
#include "engine/graphics/glerror_debug.hpp"
#include "engine/shaders/shader.hpp"

static std::string* load_shader_source_code(
	std::string const& shader_file_path
){

	std::ifstream file;
	std::stringstream code_ptr;

	// to ensure file stream can throw errors
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	
	try {
		// try to open file
		file.open(shader_file_path);
		if (!file.is_open()) return nullptr;

		// read from file
		code_ptr << file.rdbuf();

		file.close();

		return new std::string(code_ptr.str());
	}
	catch (std::ifstream::failure error) {
		return nullptr;
	}

	return nullptr;
}

static shader_object create_shader(std::string const& shader_code, GLuint shader_type) {

	shader_object shader;  shader.type = shader_type;

	// create shader
	switch (shader_type) {

		case GL_VERTEX_SHADER: {
			shader.id = glCreateShader(GL_VERTEX_SHADER);
		}break;

		case GL_FRAGMENT_SHADER: {
			shader.id = glCreateShader(GL_FRAGMENT_SHADER);
		} break;

	}

	const char* sc_ptr = shader_code.c_str();

	// compile shader
	GL_CHECK(glShaderSource(shader.id, 1, &sc_ptr, NULL));
	GL_CHECK(glCompileShader(shader.id));

	// check for errors*
	int result;
	glGetShaderiv(shader.id, GL_COMPILE_STATUS, &result);
	
	if (!result) {

		switch (shader_type) {
			case GL_VERTEX_SHADER: {
				shader.last_error = core::error::failed_to_compile_vertex_shader;
			}break;
			case GL_FRAGMENT_SHADER: {
				shader.last_error = core::error::failed_to_compile_fragment_shader;
			}break;
		}

	}

	return shader;
}

static vertex_shader create_vertex_shader(
	std::string const& shader_code, GLuint shader_type
) {
	return create_shader(shader_code , shader_type);
}

static fragment_shader create_fragment_shader(
	std::string const& shader_code, GLuint shader_type
) {
	return create_shader(shader_code , shader_type);
}

shader::shader(
	std::string const& vertex_shader_path,
	std::string const& fragement_shader_path
) {
	
	// load_image shaders source code
	std::string* vertex_shader_code    = load_shader_source_code(vertex_shader_path);
	std::string* fragement_shader_code = load_shader_source_code(fragement_shader_path);
	
	// check if loading shaders source code failed
	if (vertex_shader_code == nullptr || fragement_shader_code == nullptr) {
		this->last_error = core::error::FAILED_TO_LOAD_SHADER_FILE;

		if (vertex_shader_code    != nullptr) delete vertex_shader_code;
		if (fragement_shader_code != nullptr) delete fragement_shader_code;

		return;
	}
	

	// create shader program
	this->id = glCreateProgram();

	if (this->id == NULL) {
		this->last_error = core::error::failed_to_create_program;
		return;
	}

	// create vertex shader
	vertex_shader vertex_shader_object = create_vertex_shader(vertex_shader_code[0] , GL_VERTEX_SHADER);
	// check for errors
	if (vertex_shader_object.last_error != core::error::none) {
		this->last_error = core::error::failed_to_create_vertex_shader;
		return;
	}

	// create fragment shader
	fragment_shader fragment_shader_object = create_fragment_shader(fragement_shader_code[0] , GL_FRAGMENT_SHADER);
	// check for errors
	if (fragment_shader_object.last_error != core::error::none) {
		this->last_error = core::error::failed_to_create_fragment_shader;
		return;
	}

	// link shaders into program
	GL_CHECK(glAttachShader(this->id, vertex_shader_object.id));
	GL_CHECK(glAttachShader(this->id, fragment_shader_object.id));
	GL_CHECK(glLinkProgram(this->id));

	// check for linking errors
	int operation_result;
	glGetProgramiv(this->id, GL_LINK_STATUS, &operation_result);
	
	if (!operation_result) {
		this->last_error = core::error::failed_to_link_program;
		return;
	}
	
	// delete shaders objects
	delete vertex_shader_code;
	delete fragement_shader_code;

	if (glIsShader(vertex_shader_object.id)) {
		GL_CHECK(glDeleteShader(vertex_shader_object.id));
	}
	if (glIsShader(fragment_shader_object.id)) {
		GL_CHECK(glDeleteShader(fragment_shader_object.id));
	}
	this->last_error = core::error::none;
}

shader::~shader() {
	GL_CHECK(glDeleteProgram(this->id));
}

void shader::use() {
	GL_CHECK(glUseProgram(this->id));
}

GLint shader::get_uniform_location(std::string const& uniform_name) {
	return glGetUniformLocation( this->id, uniform_name.c_str() );
}

void shader::set_int(GLint uniform_location, int value) {
	GL_CHECK(glUniform1i( uniform_location , value ));
}

void shader::set_bool(GLint uniform_location, bool value) {
	GL_CHECK(glUniform1i( uniform_location , (int)value ));
}

void shader::set_float(GLint uniform_location, float value) {
	GL_CHECK(glUniform1f( uniform_location, value ));
}

#endif

#endif
