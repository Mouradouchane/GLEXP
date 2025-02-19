#pragma once

#ifndef SHADER_CPP
#define SHADER_CPP

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "globals.hpp"
#include "shader.hpp"

#define MIPMAP_ZERO_LEVEL 0

/*
	TODO : add GL_CHECK to all opengl functions !
*/

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
	glShaderSource(shader.id, 1, &sc_ptr, NULL);
	glCompileShader(shader.id);

	// check for errors*
	int result;
	glGetShaderiv(shader.id, GL_COMPILE_STATUS, &result);
	
	if (!result) {

		switch (shader_type) {
			case GL_VERTEX_SHADER: {
				shader.last_error = ERR::FAILED_TO_COMPILE_VERTEX_SHADER;
			}break;
			case GL_FRAGMENT_SHADER: {
				shader.last_error = ERR::FAILED_TO_COMPILE_FRAGMENT_SHADER;
			}break;
		}

	}

	return shader;
}

shader::shader(
	std::string const& vertex_shader_path,
	std::string const& fragement_shader_path
) {
	
	// load shaders source code
	std::string* vertex_shader_code    = load_shader_source_code(vertex_shader_path);
	std::string* fragement_shader_code = load_shader_source_code(fragement_shader_path);
	
	// check if loading shaders source code failed
	if (vertex_shader_code == nullptr || fragement_shader_code == nullptr) {
		this->last_error = ERR::FAILED_TO_LOAD_SHADER_FILE;

		if (vertex_shader_code    != nullptr) delete vertex_shader_code;
		if (fragement_shader_code != nullptr) delete fragement_shader_code;

		return;
	}
	

	// create shader program
	this->id = glCreateProgram();

	if (this->id == NULL) {
		this->last_error = ERR::FAILED_TO_CREATE_PROGRAM;
		return;
	}

	// create vertex shader
	shader_object vertex_shader = create_shader(vertex_shader_code[0] , GL_VERTEX_SHADER);
	// check for errors
	if (vertex_shader.last_error != ERR::NO_ERR) {
		this->last_error = ERR::FAILED_TO_CREATE_VERTEX_SHADER;
		return;
	}

	// create fragment shader
	shader_object fragment_shader = create_shader(fragement_shader_code[0] , GL_FRAGMENT_SHADER);
	// check for errors
	if (fragment_shader.last_error != ERR::NO_ERR) {
		this->last_error = ERR::FAILED_TO_CREATE_FRAGMENT_SHADER;
		return;
	}

	// link shaders into program
	glAttachShader(this->id, vertex_shader.id);
	glAttachShader(this->id, fragment_shader.id);
	glLinkProgram(this->id);

	// check for linking errors
	int operation_result;
	glGetProgramiv(this->id, GL_LINK_STATUS, &operation_result);
	
	if (!operation_result) {
		this->last_error = ERR::FAILED_TO_LINK_PROGRAM;
		return;
	}
	
	// delete used resource
	delete vertex_shader_code;
	delete fragement_shader_code;

	if(glIsShader(vertex_shader.id))   glDeleteShader(vertex_shader.id);
	if(glIsShader(fragment_shader.id)) glDeleteShader(fragment_shader.id);

	this->last_error = ERR::NO_ERR;
}

shader::~shader() {
	glDeleteProgram(this->id);
}

void shader::use() {
	glUseProgram(this->id);
}

GLint shader::get_uniform_location(std::string const& uniform_name) {
	return glGetUniformLocation( 
		this->id, uniform_name.c_str()
	);
}

void shader::set_int(GLint uniform_location, int value) {
	glUniform1i( uniform_location , value );
}

void shader::set_bool(GLint uniform_location, bool value) {
	glUniform1i( uniform_location , (int)value );
}

void shader::set_float(GLint uniform_location, float value) {
	glUniform1f( uniform_location, value );
}

#endif
