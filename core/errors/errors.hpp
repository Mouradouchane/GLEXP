#pragma once

#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <string>
#include "core/types.hpp"

/*
	enum errors contain all the errors used in all the projects/tools
*/
namespace core {

	struct error_info {
		std::string code;
		std::string  str;
		bool is_fatal;
	};

	enum class error : u16 {
		none = 0,

		// "init" ========================
		init_function_failed,

		// "shader" ========================
		failed_to_save_shader,
		failed_to_load_shader,
		failed_to_compile_shader,
		failed_to_run_shader,
		shader_crash,
		failed_to_create_program,
		failed_to_create_vertex_shader,
		failed_to_create_fragment_shader,

		// "files" ===================
		file_not_found,
		failed_to_open_file,
		failed_to_create_file,
		failed_to_delete_file,
		failed_to_write_to_file,
		failed_to_read_from_file,

		// "folders" ========================
		folder_not_found,
		failed_to_open_folder,
		failed_to_create_folder,
		failed_to_delete_folder,

		// "images" ========================
		failed_to_load_image,

		// "textures" ========================
		failed_to_load_texture,

		// "configs" ========================
		failed_to_load_ini_file,
		failed_to_save_ini_file,

		// models ========================
		failed_to_load_model,
		failed_to_process_model,
		
		// common errors
		failed_to_load_resource,
		nullptr_object,

	}; // enum class errors

	/*
		public functions
	*/
	DLL_IMPORT error_info error_to_string(core::error error_code);

} // namespace core

#endif
