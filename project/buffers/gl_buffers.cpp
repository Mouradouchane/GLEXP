#pragma once

#ifndef GL_BUFFERS_CPP
#define GL_BUFFERS_CPP

#include "gl_buffers.hpp"

// VBO constructor
vbo::vbo(vbo_data data , std::initializer_list<vbo_layout> layouts) {

	// generate
	GL_CHECK( glGenBuffers(1, &this->id) );
	// bind
	GL_CHECK( glBindBuffer(GL_ARRAY_BUFFER, this->id) );
	// copy
	GL_CHECK( glBufferData(GL_ARRAY_BUFFER, data.size, data.ptr, data.usage) );
	// describe  
	for (vbo_layout const& layout : layouts) {
		GL_CHECK( 
			glVertexAttribPointer(
				layout.index, 
				layout.size, 
				layout.type, 
				layout.normalized,
				layout.stride,
				layout.pointe
			) 
		);
		GL_CHECK( glEnableVertexAttribArray(layout.index) );
	}
	// unbind 
	glBindBuffer(GL_ARRAY_BUFFER, NULL);

}

// VBO destructor
vbo::~vbo() {
	// TODO: free buffer from gpu memory 
}

void vbo::bind() const {
	GL_CHECK( glBindBuffer(GL_ARRAY_BUFFER, this->id) );
}

void vbo::unbind() const {
	GL_CHECK( glBindBuffer(GL_ARRAY_BUFFER, 0) );
}


// VAO constructor
vao::vao(bool bind_automatically) {
	GL_CHECK( glGenVertexArrays(1, &this->id) );
	
	if (bind_automatically) glBindVertexArray(this->id);
}

// VAO destructor
vao::~vao() {
	// TODO : free vao
}

void vao::bind() const {
	GL_CHECK( glBindVertexArray(this->id) );
}

void vao::unbind() const {
	GL_CHECK( glBindVertexArray(0) );
}

// EBO constructor
ebo::ebo() {
	// TODO : implement EBO creation
}

// EBO destructor
ebo::~ebo() {
	// TODO : free ebo
}

void ebo::bind() const {

}

void ebo::unbind() const {

}

#endif