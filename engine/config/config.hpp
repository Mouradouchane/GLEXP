#pragma once

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "errors.hpp"
#include <string>

namespace config {

	// screen configs
	namespace screen {
		extern uint16_t width;
		extern uint16_t height;
		extern uint8_t  fov; // in degree
		extern bool     full_screen;

		extern uint16_t native_width;
		extern uint16_t native_height;
		extern uint16_t refresh_rate;
	}

	// mouse configs
	namespace mouse {
		extern float speed;
		extern bool  invert;
	}

	// keyboard configs
	namespace keyboard {
		extern char move_forward;
		extern char move_backward;
		extern char move_left;
		extern char move_right;
	}

	/*
		config functions
	*/
	ERR load_configs_from_file(
		std::string const& ini_file_path
	);
	void reset_all_configs_to_default();
	void reset_resolution_to_default(); 

	ERR save_configs_to_file(
		std::string const& ini_file_path
	);

}

#endif
