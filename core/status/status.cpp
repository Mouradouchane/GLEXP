#pragma once

#ifndef STATUS_CPP
#define STATUS_CPP

#include <map>

#include "core/macros.hpp"
#include "core/types.hpp"
#include "status.hpp"

static const std::map<core::error, string> errors = {
	{core::error::none , "no error"} ,

	// "init" ========================
	{core::error::init_function_failed , "init function failed"},

	// "common" ======================
	{core::error::index_out_range , "index {} out of range {}"},

	{core::error::source_equal_destination , "source '{}' is equal to destination '{}' ?"},
	{core::error::source_bigger_than_destination , "source '{}' is bigger than destination '{}' ?"},

	{core::error::nullptr_object , "pointer to object is null-pointer !"},
	{core::error::nullptr_memory , "pointer to memory is null-pointer !"},
	{core::error::nullptr_buffer , "pointer to buffer is null-pointer !"},
	{core::error::invalid_handle , "invalid handle !"},

	{core::error::failed_to_load_resource , "failed to load resource"},
	
	// "shader" ========================
	{core::error::failed_to_save_shader, "failed to save shader"},
	{core::error::failed_to_load_shader, "failed to load shader"},
	{core::error::failed_to_compile_shader,"failed to compile shader"},
	{core::error::failed_to_run_shader, "failed to run shader"},
	{core::error::shader_crash, "shader crash"},
	{core::error::failed_to_create_program,"failed to create shader program"},
	{core::error::failed_to_create_vertex_shader,"failed to create vertex shader"},
	{core::error::failed_to_create_fragment_shader,"failed to create fragment shader"},

	// "files" ===================
	{core::error::file_not_found,			"file not found"},
	{core::error::failed_to_open_file,		"failed to open file"},
	{core::error::failed_to_create_file,	"failed to create file"},
	{core::error::failed_to_delete_file,	"failed to delete file"},
	{core::error::failed_to_write_to_file,	"failed to write to file"},
	{core::error::failed_to_read_from_file, "failed to read from file"},

	// "folders" ========================
	{core::error::folder_not_found,	       "folder not found"},
	{core::error::failed_to_open_folder,   "failed to open folder"},
	{core::error::failed_to_create_folder, "failed to create folder"},
	{core::error::failed_to_delete_folder, "failed to delete folder"},

	// "configs" ========================
	{core::error::failed_to_load_ini_file, "failed to load ini file"},
	{core::error::failed_to_save_ini_file, "failed to save ini file"},

	// models ========================
	{core::error::failed_to_process_model,"failed to process model"},

}; // errors map end


static const std::map<core::warning, string> warnings = {

	// "common" ======================
	{core::warning::none , "no warning"},
	{core::warning::runtime_crash , "this will cause crash in runtime !"},
	{core::warning::segfault_crash , "this will cause segfault crash in runtime !"},
	{core::warning::allocated_with_global_memory , "{}byte allocated using 'core::global_memory' allocator ! please consider moving this allocation to the rigth place where it's belone too !"},
	{core::warning::self_assignment , "assign object to it self is probably a bug !"}

}; // warnings map end

namespace status {

	DLL_API string get_error(core::error error_code) noexcept {

		auto error_itr = errors.find(error_code);
		return ((error_itr == errors.end()) ? "error not found !" : error_itr->second);
	}

	DLL_API string get_warning(core::warning warning_code) noexcept {

		auto warning_itr = warnings.find(warning_code);
		return ((warning_itr == warnings.end()) ? "warning not found !" : warning_itr->second);
	}

} // namespace status end 

#endif