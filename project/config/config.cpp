#pragma once

#ifndef CONFIG_CPP
#define CONFIG_CPP

#include <cstdlib>
#include "ini.h"
#include "config.hpp"

#define DEFAULT_WIDTH  800
#define DEFAULT_HEIGHT 600
#define DEFAULT_FOV    100
#define DEFAULT_ASPECT_RATIO (800.0f/600.0f)
#define MIN_FOV_SUPPORTED 20
#define MAX_FOV_SUPPORTED 120
#define DEFAULT_REFRESH_RATE 30

/*
	few private function
*/
std::string round_float(float value, int precision_digits = 2){

	std::string str_value = std::to_string( value );
	return str_value.substr(0, str_value.find(".") + precision_digits);
}

static bool is_supported_resolution(uint16_t width, uint32_t height) {
	
	if (width > config::screen::native_width || height > config::screen::native_height) {
		return false;
	}

	// list of support resolution ratios
	const std::string supported_ratios[9] = { 
		"1.0", // 1:1
		"1.5", // 3:2   
		"1.3", // 4:3   
		"1.2", // 5:4   
		"1.6", // 8:5   
		"1.7", // 16:9  
		"1.6", // 16:10 
		"3.5", // 32:9  
		"2.3", // 64:27 
	};

	std::string ratio = round_float( float(width) / float(height) );

	for (uint8_t i = 0; i < 9; i++) {
		if (ratio == supported_ratios[i]) return true;
	}

	return false;
}

namespace config {

	// screen config
	namespace screen {
		uint16_t width  = DEFAULT_WIDTH;
		uint16_t height = DEFAULT_HEIGHT;
		uint8_t  fov    = 90; // in degree
		float aspect_ratio = float(width / height);
		bool  full_screen  = false;

		uint16_t native_width  = DEFAULT_WIDTH;
		uint16_t native_height = DEFAULT_HEIGHT;
		uint16_t refresh_rate  = DEFAULT_REFRESH_RATE;
	}

	// mouse configs
	namespace mouse {
		float speed  = 1.0f;
		bool  invert = false;
	}

	// keyboard configs
	namespace keyboard {
		char move_forward  = 'z';
		char move_backward = 's';
		char move_left     = 'q';
		char move_right    = 'd';
	}

	/*
		config functions
	*/

	ERR load_configs_from_file( std::string const& ini_file_path ){

		ERR result = ERR::NO_ERR;
		// create a file instance
		mINI::INIFile file(ini_file_path);
		
		// create ini structure for handling ini file 
		mINI::INIStructure configs;

		// try to read variables from ini file 
		if (!file.read(configs)) {
			result = ERR::FAILED_TO_LOAD_INI_FILE; // if failed 
		}
		else { // check and save variable in config 

			bool full_screen = bool( std::stoul(configs[""]["full_screen"]) );
			config::screen::full_screen = full_screen;
 		
			uint32_t width   = std::stoul( configs[""]["width"] );
			uint32_t height  = std::stoul( configs[""]["height"]);
			uint32_t fov     = std::stoul( configs[""]["fov"]);

			if (is_supported_resolution(width, height)) {
				config::screen::width  = width;
				config::screen::height = height;
				config::screen::aspect_ratio = (float(width) / float(height));
			}
			else result = ERR::UNSUPPORTED_RESOLUTION;

			if (fov >= MIN_FOV_SUPPORTED && fov <= MAX_FOV_SUPPORTED) {
				config::screen::fov = uint8_t(fov);
			}
			else result = ERR::UNSUPPORTED_FOV_VALUE;

		}

		return result;
	}

	// function to resource default configs in case .ini file is missing or corrupted
	void reset_all_configs_to_default() {

		// screen
		config::screen::width  = DEFAULT_WIDTH;
		config::screen::height = DEFAULT_HEIGHT;
		config::screen::aspect_ratio = float(DEFAULT_ASPECT_RATIO);
		config::screen::full_screen  = false;
		config::screen::fov = DEFAULT_FOV;
		// mouse
		config::mouse::speed  = 1.0f;
		config::mouse::invert = false;
		// keyboard

	}

	// back to DEFAULT_WIDTHxDEFAULT_HEIGHT|4:3 :)
	void reset_resolution_to_default() {
		config::screen::width  = DEFAULT_WIDTH;
		config::screen::height = DEFAULT_HEIGHT;
		config::screen::aspect_ratio = float(DEFAULT_ASPECT_RATIO);
	}

	// save current configs to ini file
	// note: file should exist !
	ERR save_configs_to_file(std::string const& ini_file_path) {
		ERR result = ERR::NO_ERR;
		
		mINI::INIFile file(ini_file_path);
		mINI::INIStructure configs;

		// read existing values 
		file.read(configs);

		// update values in structre
		configs[""].set({
			{"width"        , std::to_string(config::screen::width)},
			{"height"       , std::to_string(config::screen::height)},
			{"full_screen"  , std::to_string(config::screen::full_screen)},
			{"fov"          , std::to_string(config::screen::fov)+" ;field of view in degree"},
			{"mouse_speed"  , std::to_string(config::mouse::speed)},
			{"mouse_invert" , std::to_string(config::mouse::invert)},
			{"move_forward" , std::string(1,config::keyboard::move_forward)},
			{"move_backward", std::string(1,config::keyboard::move_backward)},
			{"move_left"    , std::string(1,config::keyboard::move_left)},
			{"move_right"   , std::string(1,config::keyboard::move_right)},
		});

		// try to write new change back to ini file
		if (!file.write(configs)) {
			result = ERR::FAILED_TO_WRITE_TO_INI_FILE;
		}
		
		return result;
	}

} 
// config namespace end

#endif
