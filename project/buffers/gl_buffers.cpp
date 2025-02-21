#pragma once

#ifndef GL_BUFFERS_CPP
#define GL_BUFFERS_CPP

#include "gl_buffers.hpp"

// VAO constructor
vao::vao(bool bind_automatically) {
	GL_CHECK(glGenVertexArrays(1, &this->id));

	if (bind_automatically) {
		GL_CHECK(glBindVertexArray(this->id));
		this->is_binded = true;
	}
}

// VAO destructor
vao::~vao() {
	//this->unbind();
	//GL_CHECK(glDeleteVertexArrays(1, &this->id));
}

void vao::bind() {
	GL_CHECK( glBindVertexArray(this->id) );
	this->is_binded = true;
}

void vao::unbind() {
	GL_CHECK( glBindVertexArray(0) );
	this->is_binded = false;
}



// VBO constructor
vbo::vbo(vbo_data data, std::initializer_list<vbo_data_layout> layouts) {

	// generate
	GL_CHECK( glGenBuffers(1, &this->id) );
	// bind
	GL_CHECK( glBindBuffer(GL_ARRAY_BUFFER, this->id) );
	// copy
	GL_CHECK( glBufferData(GL_ARRAY_BUFFER, data.size, data.ptr, data.usage) );
	
	// describe  
	for (vbo_data_layout const& layout : layouts) {

		GL_CHECK( glEnableVertexAttribArray(layout.index) );
		GL_CHECK( 
			glVertexAttribPointer(
				layout.index, 
				layout.size, 
				layout.type, 
				layout.normalized,
				layout.stride,
				layout.pointer
			) 
		);
	}
	// unbind 
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

// VBO destructor
vbo::~vbo() {
	//this->unbind();
	//GL_CHECK( glDeleteBuffers(1, &this->id) );
}

void vbo::bind() {
	GL_CHECK( glBindBuffer(GL_ARRAY_BUFFER, this->id) );
	this->is_binded = true;
}

void vbo::unbind() {
	GL_CHECK( glBindBuffer(GL_ARRAY_BUFFER, 0) );
	this->is_binded = false;
}



// EBO constructor
ebo::ebo(std::vector<uint32_t> const& indices) {
	
	// create
	GL_CHECK( glGenBuffers(1, &this->id) );
	// bind
	GL_CHECK( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id) );
	// copy
	GL_CHECK( 
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER, 
			indices.size()*sizeof(uint32_t), 
			&indices[0], 
			GL_STATIC_DRAW
		)
	);

	// unbind
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

// EBO destructor
ebo::~ebo() {
	//this->unbind();
	//GL_CHECK( glDeleteBuffers(1, &this->id) );
}

void ebo::bind(){
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id));
	this->is_binded = true;
}

void ebo::unbind(){
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	this->is_binded = false;
}

#endif