#pragma once 

#ifndef MODELS_CPP
#define MODELS_CPP

#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing flags

#include "models.hpp"

mesh_data::mesh_data(
	std::vector<vertex>& _vertices,
	std::vector<uint32_t>& _indices,
	std::vector<texture_2d>& _textures
) {
	this->vertices = _vertices;
	this->indices  = _indices;
	this->textures = _textures;
}

// model destructor
model::~model() {
	for (mesh const& _mesh : this->meshs) {
		_mesh.~mesh();
	}
}

// mesh destructor
mesh::~mesh() {
	// TODO: delete buffers from gpu memory buffers
}

mesh::mesh(mesh_data* data) {
	
	this->VAO = create_vao(true);
	
	this->VBO = create_vbo(
			&data->vertices[0] , 
			uint32_t(data->vertices.size() * sizeof(vertex)),
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
		&data->indices[0], 
		uint32_t(sizeof(uint32_t)*data->indices.size()), 
		GL_STATIC_DRAW
	);
	//bind_ebo(this->EBO);
	this->indices_size = data->indices.size();

	unbind_vao(this->VAO);
}

void mesh::bind() {
	bind_vao(this->VAO);
	bind_ebo(this->EBO);
}
void mesh::unbind() {
	bind_ebo(0);
	bind_vao(0);
}

mesh_data* process_mesh(aiMesh* mesh , const aiScene* scene) {

	if (mesh == nullptr) return nullptr;

	mesh_data* data = new mesh_data();
	// allocate space for mesh data
	data->vertices = std::vector<vertex>(mesh->mNumVertices);

	// copy vertices 
	for (uint32_t i = 0; i < mesh->mNumVertices; i++) {

		data->vertices[i].position = vec3(
			mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z
		);
		data->vertices[i].normal = vec3(
			mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z
		);

		if (mesh->mTextureCoords[0]) {
			data->vertices[i].texture_coord = vec2(
				mesh->mTextureCoords[0][i].x , mesh->mTextureCoords[0][i].y
			);
		}
	}

	// copy indices
	for(uint32_t i = 0; i < mesh->mNumFaces; i++){
		aiFace face = mesh->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; j++) {
			data->indices.push_back(face.mIndices[j]);
		}
	}

	// TODO : process material 

	return data;
}

void process_node(aiNode* node , const aiScene* scene , model* _model) {

	if (node == nullptr) return;

	// process current mesh in node
	for (uint32_t i = 0; i < node->mNumMeshes; i++) {
		aiMesh* _mesh = scene->mMeshes[ node->mMeshes[i] ];
		mesh_data* data = process_mesh(_mesh, scene);

		if (data != nullptr) {
			// mesh model_mesh = mesh(data);
			_model->meshs.push_back( mesh(data) );
		}

		delete data;
	}
	// process current node childern nodes
	for (uint32_t i = 0; i < node->mNumChildren; i++) {
		process_node(node->mChildren[i], scene, _model);
	}

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
	if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::string assimp_error(importer.GetErrorString());
		// TODO : show error message 
		return ERR::FAILED_TO_PROCESS_MODEL;
	}

	// process model
	process_node(scene->mRootNode, scene, dest_model_object);

	return ERR::NO_ERR;
}



#endif