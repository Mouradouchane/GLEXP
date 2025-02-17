#pragma once

#ifndef GL_BUFFERS_HPP
#define GL_BUFFERS_HPP

#include <stdint.h>
#include <initializer_list>
#include "glew/glew.h"
#include "glerror_debug.hpp"

struct vbo_data {
	void*    ptr   = nullptr;
	uint32_t size  = 0;
	GLenum   usage = GL_STATIC_DRAW;
};

struct vbo_layout {
	GLuint 			index;
	GLint 			size;
	GLenum 			type = GL_FLOAT;
	GLboolean 		normalized = GL_FALSE;
	GLsizei			stride = 0;
	const GLvoid*	pointe = nullptr;
};

class vbo {
private:
	// VBO handle
	GLuint id = 0; 

public:
	// constructor's
	vbo() = default;
	vbo(vbo_data data , std::initializer_list<vbo_layout> layouts);
	// destructor
	~vbo();

	// methods
	void bind() const;
	void unbind() const;
};

// TODO : implement EBO
class ebo {
private:
	// EBO handle 
	GLuint id; 

public:
	ebo();
	~ebo();

	void bind() const;
	void unbind() const;
};

class vao {
public :
	// VAO handle
	GLuint id = 0; 
	
	// constructor
	vao(bool bind_automatically = false);
	// destructor
	~vao();

	// methods 
	void bind() const;
	void unbind() const;
};

#endif