#pragma once

#ifndef STATUS_HPP
#define STATUS_HPP

#include "core/strings/string.hpp"
#include "core/types.hpp"

namespace core {

	// todo: add maps for errors and warning !
	//       map<error , string> errors_list ;
	//       map<warning , string> warnings_list ;
	// todo: give the ability to add custom errors/warnings 

	/*
		contain all the errors used core/engine/tools/...
	*/
	enum class error : u16 {
		none = 0,

		// common errors
		index_out_range,
		source_equal_destination,
		source_bigger_than_destination,
		nullptr_object,
		nullptr_memory,
		nullptr_buffer,
		failed_to_load_resource,

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

	}; // enum class errors


	enum class warning : u16 {
		none = 0,
		allocated_with_global_memory,
		self_assignment,
	};

	namespace status {

		DLL_API string get_error(core::error error_code) noexcept;
		DLL_API string get_warning(core::warning warning_code) noexcept;
	
	} // namespace status end 

} // namespace core end 

#endif
