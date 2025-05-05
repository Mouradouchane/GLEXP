#pragma once 

#ifndef RESOURCE_MANAGER_CPP
#define RESOURCE_MANAGER_CPP

#include <vector>
#include "assert.hpp"
#include "resource_manager.hpp"

namespace resource {
	
namespace {
	std::vector<model>      models;
	std::vector<texture_2d> textures;
	//std::vector<shader>   shaders;
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

ERR load_resources( std::string const& resources_list_file_path ) {
	
	ini_struct resource_ini;

	EXIT_AT_ERR(
		load_ini_file(resources_list_file_path, resource_ini) , 
		std::string("failed to load: " + resources_list_file_path)
	);

	// load models
	if (resource_ini.has("models")) {
		resource::load_models(resource_ini);
	}

	// load textures 
	if (resource_ini.has("textures")) {
		//resource::load_textures(resource_ini);
	}
	
	// load shaders
	if (resource_ini.has("shaders")) {
		//resource::load_shaders(resource_ini);
	}

	return ERR::NO_ERR;
}

// TODO:
ERR load_model( std::string const& file_path ){

	ASSERT_EXP(0);

	return ERR::NO_ERR;
}

// TODO:
ERR load_models( ini_struct const& resource_ini ) {

	auto const& collection = resource_ini.get("models");

	// try to load all the models 
	for (auto const& itr : collection) {

		const std::string model_path = itr.first;
		EXIT_AT_ERR(
			resource::load_model( model_path ), 
			std::string("failed to load model: " + model_path) 
		);
	}

	return ERR::NO_ERR;
}


}
// namespace resource end

#endif