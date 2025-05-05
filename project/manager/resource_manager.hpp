#pragma once 

#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <vector>
#include <string>
#include "ini.h"
#include "errors.hpp"
#include "models.hpp"

typedef mINI::INIFile ini_file;
typedef mINI::INIStructure ini_struct;

namespace resource {

	// load ini structre from file
	ERR load_ini_file(
		std::string const& file_path,
		ini_struct& dest_ini_object
	);

	ERR load_resources(
		std::string const& resources_list_file_path
	);
	
	// to load a single model
	ERR load_model(
		std::string const& file_path
	);

	// to load a list of models 
	ERR load_models(
		ini_struct const& resource_ini
	);

	ERR load_texture(
		std::string const& file_path,
		model& dest_texture_object
	);

};

#endif