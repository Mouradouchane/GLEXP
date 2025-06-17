#pragma once 

#ifndef RESOURCE_MANAGER_CPP
#define RESOURCE_MANAGER_CPP

#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output new_mesh structure
#include "assimp/postprocess.h"     // Post processing flags

#include "assert.hpp"
#include "resource_manager.hpp"

/*
	few static functions for processing 3D models and make them ready to use
*/
static mesh* process_mesh( aiMesh* ai_mesh, const aiScene* ai_scene );
static void  process_node( aiNode* ai_node, const aiScene* ai_scene, model* p_model );

namespace resource {
	
namespace {
	std::vector<image>    images(128);
	std::vector<texture>  textures(128);
	std::vector<model>    models(64);
    //std::vector<shader> shaders;
}

ERR load_ini_file(
	std::string const& file_path, ini_struct& dest_ini_object
){
	mINI::INIFile file(file_path);

	// if failed to load variables from ini file 
	if (!file.read(dest_ini_object)) {
		return ERR::FAILED_TO_LOAD_INI_FILE; 
	}

	return ERR::NO_ERR;
}

// todo change return ERR to "engine status"
ERR load_resources(
	std::string const& resources_map_file_path
){
	ini_struct resources;
	
	EXIT_AT_ERR(
		load_ini_file(resources_map_file_path, resources) , 
		std::string("failed to load: " + resources_map_file_path)
	);

	DEBUG_BREAK;

	// load models
	if (resources.has("models")) {
		resource::load_models( resources.get("models") );
	}

	// load images
	if (resources.has("textures")) {
		resource::load_textures( resources.get("textures") );
	}

	// load textures 
	if (resources.has("textures")) {
		resource::load_textures( resources.get("textures") );
	}
	
	// load shaders
	if (resources.has("shaders")) {
		//resource::load_shaders( resources.get("shaders") );
	}

	return ERR::NO_ERR;
}


ERR load_texture(
	std::string const& texture_file_path,
	texture* destination
) {
	DEBUG_BREAK;

	image _image(texture_file_path , "" , true);

	if (_image.get_last_error() != ERR::NO_ERR) {
		return ERR::FAILED_TO_LOAD_TEXTURE;
	}
	if (_image.buffer() == nullptr) {
		return ERR::TEXTURE_BUFFER_NULLPTR;
	}

	*destination = texture( &_image );

	return ERR::NO_ERR;
}

ERR load_textures(
	resource_map const& textures_map
) {
	DEBUG_BREAK;

	// if textures vector memory not enough 
	if (textures_map.size() > textures.size()) {
		textures.resize( 
			textures.size() + (textures_map.size() - textures.size()) 
		);
	}

	// TODO: implement multi-threaded loading later
	size_t position = 0;
	for (auto const& item : textures_map) {

		ERR result = resource::load_texture(item.second , &resource::textures[position]);
		
		if (result != ERR::NO_ERR) {
			// TODO: display texture loading error in game console
		}
		else position += 1;
	}

	return ERR::NO_ERR;
}

// NOTE: models gonna be in RAM not GPU-MEMORY
ERR load_model( 
	std::string const& model_file_path, model* destination 
){
	// DEBUG_BREAK;
	if (destination == nullptr) {
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

	// if importer failed to load_image file
	if (ai_scene == nullptr) {
		return ERR::FAILED_TO_LOAD_MODEL;
	}

	// if failed to process model
	if (ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) {
		// TODO : show error message in game console FUTURE
		std::string assimp_error(importer.GetErrorString());

		return ERR::FAILED_TO_PROCESS_MODEL;
	}

	// process model by starting from "root ai_node"
	process_node(ai_scene->mRootNode, ai_scene, destination);

	return ERR::NO_ERR;
}

ERR load_models( 
	resource_map const& models_map 
){
	DEBUG_BREAK;

	// if models vector memory not enough 
	if (models_map.size() > models.size()) {
		models.resize( models.size() + (models_map.size() - models.size()) );
	}
	
	// loading the models one by one 
	// TODO: multi-threading models loading later :)
	uint32_t position = 0;
	for (auto const& itr : models_map) {

		const std::string model_path = itr.second;

		RET_ERR(resource::load_model(model_path, &resource::models[position]));
		resource::models[position].name = itr.first;

		// if models vector need resize
		if (position >= models.size()) {
			models.resize( models.size() + size_t(models.size() * 0.5));
		}

		position += 1;
	}

	return ERR::NO_ERR;
}


}
/*
	============== resource namespace end ================
*/


/*
	============== model processing functions ==============
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

	// TODO: process materials
	
	// TODO: maybe process textures

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