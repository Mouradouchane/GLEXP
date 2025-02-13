#pragma once 

#ifndef MODELS_CPP
#define MODELS_CPP

#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing flags

#include "models.hpp"

namespace models_loader {

	void test_assimp(std::string const& file_path) {

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile( 
			file_path.c_str(),
			aiProcess_CalcTangentSpace       |
			aiProcess_Triangulate            |
			aiProcess_JoinIdenticalVertices  |
			aiProcess_SortByPType
		);

		// importer failed to load file
		if (scene == nullptr) {
			// throw error
		}
		// Now we can access the file's contents.
		
	}

}

#endif