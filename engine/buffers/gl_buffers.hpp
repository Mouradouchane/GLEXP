#pragma once

#ifndef GL_BUFFERS_HPP
#define GL_BUFFERS_HPP

#include <stdint.h>
#include <initializer_list>
#include "glerror_debug.hpp"
#include "glew/glew.h"


struct vbo_data_layout {
	GLuint 			index;
	GLint 			size;
	GLenum 			type = GL_FLOAT;
	GLboolean 		normalized = GL_FALSE;
	GLsizei			stride = 0;
	const GLvoid*	pointer = nullptr;
};

typedef GLuint vao;
typedef GLuint vbo;
typedef GLuint ebo;

vao create_vao(bool bind_automatically);

vbo create_vbo(
	void* buffer, uint32_t size, GLenum usage,
	std::initializer_list<vbo_data_layout> layouts
);

ebo create_ebo(
	void* buffer, uint32_t size, GLenum usage
);

void bind_vao(vao VAO_ID);
void bind_vbo(vbo VBO_ID);
void bind_ebo(ebo EBO_ID);

void unbind_vao(vao VAO_ID);
void unbind_vbo(vbo VBO_ID);
void unbind_ebo(ebo EBO_ID);

void delete_vao(vao VAO_ID);
void delete_vbo(vbo VBO_ID);
void delete_ebo(ebo EBO_ID);

#endif