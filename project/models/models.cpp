#pragma once 

#ifndef MODELS_CPP
#define MODELS_CPP

#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output new_mesh structure
#include "assimp/postprocess.h"     // Post processing flags

#include "models.hpp"

/*
	few static functions for processing assimp meshes and models
*/
static mesh* process_mesh(
	aiMesh* ai_mesh, const aiScene* ai_scene
);
static void process_node(
	aiNode* ai_node, const aiScene* ai_scene, model* p_model
);


/*
	============== mesh ==============
*/

mesh::mesh(
	std::string const& mesh_name,
	std::vector<vertex>& _vertices,
	std::vector<uint32_t>& _indices,
	std::vector<texture_2d>& _textures
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
		delete ptr_mesh;
	}
}

ERR model::load_model(
	std::string const& model_file_path, model* model_destination
){

	if (model_destination == nullptr) {
		return ERR::NULLPTR_MODEL_OBJET;
	}
	
	Assimp::Importer importer;

	const aiScene* ai_scene = importer.ReadFile( 
		model_file_path.c_str(),
		//aiProcess_CalcTangentSpace     |
		aiProcess_Triangulate            |
		aiProcess_JoinIdenticalVertices  |
		//aiProcess_SortByPType          |
		aiProcess_GenNormals			 |
		aiProcess_FlipUVs
	);

	// if importer failed to resource file
	if (ai_scene == nullptr) {
		// TODO : show error message

		return ERR::FAILED_TO_LOAD_MODEL;
	}

	// if failed to process model
	if (ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) {
		// TODO : show error message 
		std::string assimp_error(importer.GetErrorString());

		return ERR::FAILED_TO_PROCESS_MODEL;
	}

	// process model by starting from "root ai_node"
	process_node(ai_scene->mRootNode, ai_scene, model_destination);

	return ERR::NO_ERR;
}
// function load_model end

// TODO: implement loading multiple models 
static ERR load_models(
	std::vector<std::string> const& models_file_path,
	std::vector<model>& models_destination
) {

	return ERR::NO_ERR;
}

/*
	============== process functions ==============
*/

static mesh* process_mesh(
	aiMesh* ai_mesh , const aiScene* ai_scene
) {
	if (ai_mesh == nullptr) return nullptr;

	mesh* new_mesh = new mesh();
	// allocate space for ai_mesh new_mesh
	new_mesh->vertices = std::vector<vertex>(ai_mesh->mNumVertices);

	// copy vertices 
	for (uint32_t i = 0; i < ai_mesh->mNumVertices; i++) {

		new_mesh->vertices[i].position = vec3(
			ai_mesh->mVertices[i].x, ai_mesh->mVertices[i].y, ai_mesh->mVertices[i].z
		);
		new_mesh->vertices[i].normal = vec3(
			ai_mesh->mNormals[i].x, ai_mesh->mNormals[i].y, ai_mesh->mNormals[i].z
		);

		if (ai_mesh->mTextureCoords[0]) {

			new_mesh->vertices[i].texture_coord = vec2(
				ai_mesh->mTextureCoords[0][i].x , ai_mesh->mTextureCoords[0][i].y
			);
		}
	}

	// copy indices
	for(uint32_t i = 0; i < ai_mesh->mNumFaces; i++){
		aiFace face = ai_mesh->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; j++) {
			new_mesh->indices.push_back(face.mIndices[j]);
		}
	}

	// TODO: process material 

	return new_mesh;
}

static void process_node(
	aiNode* ai_node, const aiScene* ai_scene, model* p_model
){

	if (ai_node == nullptr) return;

	// process current mesh in ai_node
	for (uint32_t i = 0; i < ai_node->mNumMeshes; i++) {

		aiMesh* ai_mesh = ai_scene->mMeshes[ ai_node->mMeshes[i] ];
		mesh*  new_mesh = process_mesh(ai_mesh, ai_scene);

		if (new_mesh != nullptr) {
			p_model->meshes.push_back(new_mesh);
		}
	}

	// process current ai_node childern nodes
	for (uint32_t i = 0; i < ai_node->mNumChildren; i++) {
		process_node(ai_node->mChildren[i], ai_scene, p_model);
	}

}

#endif