#pragma once

#ifndef GL_BUFFERS_HPP
#define GL_BUFFERS_HPP

#include <stdint.h>
#include <initializer_list>
#include <vector>
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
	bool   is_binded = false;

public:
	// constructor's
	vbo() = default;
	vbo(vbo_data data, std::initializer_list<vbo_layout> layouts);
	// destructor
	~vbo();

	// methods
	void bind();
	void unbind();
};

// TODO : implement EBO
class ebo {
private:
	// EBO handle 
	GLuint id; 
	bool   is_binded = false;

public:
	// constructors
	ebo() = default;
	ebo(std::vector<uint32_t> const& indices);
	// destructor
	~ebo();

	void bind();
	void unbind();
};

class vao {
public :
	// VAO handle
	GLuint id = 0; 
	bool   is_binded = false;
	
	// constructor
	vao() = default;
	vao(bool bind_automatically);
	// destructor
	~vao();

	// methods 
	void bind();
	void unbind();
};

#endif