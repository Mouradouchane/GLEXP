#pragma once

#ifndef GL_BUFFERS_HPP
#define GL_BUFFERS_HPP

#include <stdint.h>
#include "glew/glew.h"

class vbo {
public:
	// VBO handle
	GLuint id; 

	// constructor's
	vbo() = default;
	vbo(void* data , uint32_t size);
	// destructor
	~vbo();

	// methods
	void bind();
	void unbind();
};

// TODO : implement EBO
struct ebo {
	// EBO handle 
	GLuint id; 

	ebo() = default;
	~ebo();

	void bind();
	void unbind();
};

struct vao {
	// VAO handle
	GLuint id; 

	vao() = default;
	~vao();
	void bind();
	void unbind();
};


#endif