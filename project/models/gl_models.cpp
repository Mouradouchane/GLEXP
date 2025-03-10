#pragma once 

#ifndef GL_MODELS_CPP
#define GL_MODELS_CPP

#include "glew/glew.h"
#include "gl_models.hpp"

/*
	=========== gl_mesh ===========
*/

gl_mesh::gl_mesh(mesh* mesh_) {

	this->name = std::string(mesh_->name);

	this->VAO = create_vao(true);

	this->VBO = create_vbo(
		&mesh_->vertices[0] , 
		uint32_t(mesh_->vertices.size() * sizeof(vertex)),
		GL_STATIC_DRAW,
		{ 
			// vertex layout
			vbo_data_layout{0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)0}, 
			// normal layout
			vbo_data_layout{1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex,normal)}, 
			// uv layout
			vbo_data_layout{2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex,texture_coord)}  
		}
	);

	this->EBO = create_ebo( 
		&mesh_->indices[0], 
		uint32_t(sizeof(uint32_t)*mesh_->indices.size()), 
		GL_STATIC_DRAW
	);

	this->indices_size = mesh_->indices.size();

	unbind_vao(this->VAO);
}

gl_mesh::~gl_mesh() {
	this->name.~basic_string();

	// deallocate mesh from gpu memory
	delete_vbo(this->VBO);
	delete_ebo(this->EBO);
	delete_vao(this->VAO);
}

void gl_mesh::bind() {
	bind_vao(this->VAO);
	bind_ebo(this->EBO);
}
void gl_mesh::unbind() {
	bind_ebo(0);
	bind_vao(0);
}

// TODO: implement rendering using shader
void gl_mesh::render(shader const& shader_program) {

}

/*
	=========== gl_model ===========
*/

// TODO: implement model constructor
gl_model::gl_model(model* model_) {

}

gl_model::~gl_model() {
	this->name.~basic_string();

	for (gl_mesh const& _mesh : this->meshes) {
		_mesh.~gl_mesh();
	}
}

// TODO: implement render using shader
void gl_model::render(shader const& shader_program) {

}


#endif