#pragma once 

#ifndef MODELS_CPP
#define MODELS_CPP

#include "models.hpp"

/*
	============== mesh ==============
*/

mesh::mesh(
	std::string const& mesh_name,
	std::vector<vertex>& _vertices,
	std::vector<uint32_t>& _indices,
	std::vector<texture>& _textures
) {
	this->name     = std::string(mesh_name);
	this->vertices = _vertices;
	this->indices  = _indices;
	this->textures = _textures;
}


/*
	============== model ==============
*/

model::~model() {
	for (mesh* ptr_mesh : this->meshes) {
		if(ptr_mesh != nullptr) delete ptr_mesh;
	}
}

#endif