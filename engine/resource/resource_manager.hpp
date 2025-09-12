#pragma once 

#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <vector>
#include <string>
#include "libs/ini.h"
#include "engine/core/errors/errors.hpp"
#include "engine/textures/image.hpp"
#include "engine/textures/texture.hpp"
#include "engine/models/models.hpp"

typedef mINI::INIFile ini_file;
typedef mINI::INIStructure ini_struct;
typedef mINI::INIMap<std::string> resource_map;

namespace resource{

	// load ini structre from file
	ERR load_ini_file(
		std::string const& file_path,
		ini_struct& dest_ini_object
	);

	ERR load_resources(
		std::string const& resources_list_file_path
	);
	
	/*
		functions for loading 1 asset
	*/
	ERR load_model(
		std::string const& model_file_path,
		model* destination
	);

	ERR load_image(
		std::string const& image_file_path ,
		image* destination
	);

	ERR load_texture( 
		std::string const& texture_file_path,
		texture* destination
	);


	/*
		functions for loading multiple assets
	*/

	ERR load_models(
		resource_map const& models_map
	);

	ERR load_images(
		resource_map const& images_map 
	);

	ERR load_textures(
		resource_map const& textures_map
	);

/* 
	// TODO: implement shaders loading
	ERR load_shader( 
	std::string const& shader_file_path,
	shader* destination
	);
*/
};

#endif