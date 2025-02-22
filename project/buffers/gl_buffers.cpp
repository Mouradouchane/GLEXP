#pragma once

#ifndef GL_BUFFERS_CPP
#define GL_BUFFERS_CPP

#include "gl_buffers.hpp"

/*
	VAO functions
*/

vao create_vao(bool bind_automatically) {
	vao id = 0;
	GL_CHECK(glGenVertexArrays(1, &id));

	if (bind_automatically) {
		GL_CHECK(glBindVertexArray(id));
	}

	return id;
}

 void bind_vao(vao VAO_ID) {
	GL_CHECK( glBindVertexArray(VAO_ID) );
}
 void unbind_vao(vao VAO_ID) {
	GL_CHECK( glBindVertexArray(0) );
}

 void delete_vao(vao VAO_ID) {
	if (glIsVertexArray(VAO_ID) == GL_TRUE) {
		GL_CHECK( glDeleteVertexArrays(1, &VAO_ID) );
	}
}


/*
	VBO functions
*/

vbo create_vbo(
	void* buffer, uint32_t size, GLenum usage,
	std::initializer_list<vbo_data_layout> layouts
) {
	vbo id = 0;

	// generate
	GL_CHECK( glGenBuffers(1, &id) );
	// bind
	GL_CHECK( glBindBuffer(GL_ARRAY_BUFFER, id) );
	// copy
	GL_CHECK( glBufferData(GL_ARRAY_BUFFER, size, buffer, usage) );

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

	return id;
}

 void bind_vbo(vbo VBO_ID) {
	GL_CHECK( glBindBuffer(GL_ARRAY_BUFFER, VBO_ID) );
}

 void unbind_vbo(vbo VBO_ID) {
	GLint current_binded_buffer = 0;
	GL_CHECK(glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &current_binded_buffer));
	
	if (current_binded_buffer == VBO_ID) {
		GL_CHECK( glBindBuffer(GL_ARRAY_BUFFER, 0) );
	}
}

 void delete_vbo(vbo VBO_ID) {
	if (glIsBuffer(VBO_ID) == GL_TRUE) {
		GL_CHECK( glDeleteBuffers(1, &VBO_ID) );
	}
}


/*
	EBO functions
*/

ebo create_ebo(
	void* buffer, uint32_t size, GLenum usage
) {
	ebo id = 0;
	// create
	GL_CHECK( glGenBuffers(1, &id) );
	// bind
	GL_CHECK( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id) );
	// copy
	GL_CHECK(
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER, 
			size, 
			buffer, 
			usage
		)
	);

	// unbind
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	
	return id;
}

 void bind_ebo(ebo EBO_ID) {
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_ID));
}
 void unbind_ebo(ebo EBO_ID) {
	GLint current_binded_EBO = 0;
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &current_binded_EBO);

	if (current_binded_EBO == EBO_ID) {
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
}

 void delete_ebo(ebo EBO_ID) {
	if (glIsBuffer(EBO_ID) == GL_TRUE) {
		GL_CHECK( glDeleteBuffers(1, &EBO_ID) );
	}
}

#endif