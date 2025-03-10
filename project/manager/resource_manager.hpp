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

	ERR load_ini_file(
		std::string const& file_path,
		ini_struct& dest_ini_object
	);

	ERR load_paths_list_from_file(
		std::string const& file_path,
		std::vector<std::string>& dest_list
	);

	ERR load_resource_list(std::string const& file_path);

	ERR load_model(
		std::string const& file_path,
		model& dest_model_object
	);

	ERR load_models(
		std::vector<std::string> const& models_path_list,
		std::vector<model>& dest_models
	);

	ERR load_texture(
		std::string const& file_path,
		model& dest_texture_object
	);

};

#endif