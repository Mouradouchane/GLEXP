#pragma once

#ifndef GL_BUFFERS_HPP
#define GL_BUFFERS_HPP

#include <stdint.h>
#include <initializer_list>
#include <vector>
#include "glerror_debug.hpp"
#include "glew/glew.h"

struct vbo_data {
	void*    ptr   = nullptr;
	uint32_t size  = 0;
	GLenum   usage = GL_STATIC_DRAW;
};

struct vbo_data_layout {
	GLuint 			index;
	GLint 			size;
	GLenum 			type = GL_FLOAT;
	GLboolean 		normalized = GL_FALSE;
	GLsizei			stride = 0;
	const GLvoid*	pointer = nullptr;
};

/*
typedef GLuint vao;
typedef GLuint vbo;
typedef GLuint ebo;

namespace bind {

	inline void array_object(vao VAO_ID);
	inline void buffer_object(vbo VBO_ID);
	inline void element_object(vbo EBO_ID);

}

namespace unbind {

	inline void array_object();
	inline void buffer_object();
	inline void element_object();

}
*/

struct vbo {
	
	// VBO handle
	GLuint id = 0; 
	bool   is_binded = false;

	// constructor's
	vbo() = default;
	vbo(vbo_data data, std::initializer_list<vbo_data_layout> layouts);
	// destructor
	~vbo();

	// methods
	void bind();
	void unbind();
};


struct ebo {
	// EBO handle 
	GLuint id; 
	bool   is_binded = false;

	// constructors
	ebo() = default;
	ebo(std::vector<uint32_t> const& indices);
	// destructor
	~ebo();

	void bind();
	void unbind();
};


struct vao {

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