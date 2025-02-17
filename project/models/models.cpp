#pragma once 

#ifndef MODELS_CPP
#define MODELS_CPP

#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing flags
#include "glm.hpp"
#include "models.hpp"

typedef glm::vec3 vec3;
typedef glm::vec2 vec2;

struct vertex {
	vec3 position;
	vec3 normal;
	vec2 texture_coord;
};

// model destructor
model::~model() {
	// TODO: delete meshes from gpu memory and free model
}

// mesh destructor
mesh::~mesh() {
	// TODO: delete buffers from gpu memory buffers
}

ERR model::load_model(
	std::string const& model_file_path , 
	model* dest_model_object
){
	if (dest_model_object == nullptr) {
		return ERR::NULLPTR_MODEL_OBJET;
	}

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile( 
		model_file_path.c_str(),
		aiProcess_CalcTangentSpace       |
		aiProcess_Triangulate            |
		aiProcess_JoinIdenticalVertices  |
		aiProcess_SortByPType
	);

	// if importer failed to load file
	if (scene == nullptr) {
		// show error message "maybe"
		return ERR::FAILED_TO_LOAD_MODEL;
	}

	// process model


	return ERR::NO_ERR;
}



#endif