#pragma once 

#ifndef RESOURCE_MANAGER_CPP
#define RESOURCE_MANAGER_CPP

#include "assert.hpp"
#include "resource_manager.hpp"

namespace resource {

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

ERR load_paths_list_from_file(
	std::string const& file_path,
	std::vector<std::string>& dest_list
){

	mINI::INIStructure ini_structure;
	ASSERT_ERR(
		resource::load_ini_file(file_path,ini_structure)
	);

	for (auto const& it : ini_structure){
		auto const& section = it.first;
		auto const& collection = it.second;

		for (auto const& it2 : collection){
			auto const& key = it2.first;
			// auto const& value = it2.second;

			dest_list.push_back(key);
		}

	}

	return ERR::NO_ERR;
}

// TODO: 
ERR load_models(
	std::vector<std::string> const& models_path_list,
	std::vector<model>& dest_models
) {

	return ERR::NO_ERR;
}

// TODO: 
ERR load_model(
	std::string const& file_path, model& dest_model_object
) {

	return ERR::NO_ERR;
}

}
// namespace resource end

#endif