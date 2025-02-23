#pragma once 

#ifndef MODELS_CPP
#define MODELS_CPP

#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing flags

#include "models.hpp"

/*
	few static functions for processing assimp meshes and models
*/
static void  process_node(aiNode* node, const aiScene* scene, model* model_);
static mesh* process_mesh(aiMesh* assimp_mesh, const aiScene* scene);

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

ERR load_model(
	std::string const& model_file_path, model* destination_model
){

	if (destination_model == nullptr) {
		return ERR::NULLPTR_MODEL_OBJET;
	}
	
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile( 
		model_file_path.c_str(),
		//aiProcess_CalcTangentSpace     |
		aiProcess_Triangulate            |
		aiProcess_JoinIdenticalVertices  |
		//aiProcess_SortByPType          |
		aiProcess_GenNormals			 |
		aiProcess_FlipUVs
	);

	// if importer failed to load file
	if (scene == nullptr) {
		// TODO : show error message

		return ERR::FAILED_TO_LOAD_MODEL;
	}
	// if failed to process model
	if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		// TODO : show error message 
		std::string assimp_error(importer.GetErrorString());

		return ERR::FAILED_TO_PROCESS_MODEL;
	}

	// process model by starting from "root node"
	process_node(scene->mRootNode, scene, destination_model);

	return ERR::NO_ERR;
}


mesh* process_mesh(aiMesh* gl_mesh , const aiScene* scene) {

	if (gl_mesh == nullptr) return nullptr;

	mesh* data = new mesh();
	// allocate space for gl_mesh data
	data->vertices = std::vector<vertex>(gl_mesh->mNumVertices);

	// copy vertices 
	for (uint32_t i = 0; i < gl_mesh->mNumVertices; i++) {

		data->vertices[i].position = vec3(
			gl_mesh->mVertices[i].x, gl_mesh->mVertices[i].y, gl_mesh->mVertices[i].z
		);
		data->vertices[i].normal = vec3(
			gl_mesh->mNormals[i].x, gl_mesh->mNormals[i].y, gl_mesh->mNormals[i].z
		);

		if (gl_mesh->mTextureCoords[0]) {
			data->vertices[i].texture_coord = vec2(
				gl_mesh->mTextureCoords[0][i].x , gl_mesh->mTextureCoords[0][i].y
			);
		}
	}

	// copy indices
	for(uint32_t i = 0; i < gl_mesh->mNumFaces; i++){
		aiFace face = gl_mesh->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; j++) {
			data->indices.push_back(face.mIndices[j]);
		}
	}

	// TODO : process material 

	return data;
}

static void process_node(
	aiNode* node, const aiScene* scene, model* model_
){

	if (node == nullptr) return;

	// process current mesh in node
	for (uint32_t i = 0; i < node->mNumMeshes; i++) {
		aiMesh* _mesh = scene->mMeshes[ node->mMeshes[i] ];
		mesh* data = process_mesh(_mesh, scene);

		if (data != nullptr) {
			model_->meshes.push_back(data);
		}

	}
	// process current node childern nodes
	for (uint32_t i = 0; i < node->mNumChildren; i++) {
		process_node(node->mChildren[i], scene, model_);
	}

}

#endif