#pragma once

#ifndef IMAGE_HPP
#define IMAGE_HPP

/*
	todo: rework
*/

#if 0

#include "core/status/status.hpp"
#include <string>

class image {

private:
	// propertie's
	uint8_t* data = nullptr;
	int channle = -1;

	std::string name = "no-title";
	std::string path = "";

	uint16_t width   = 0; 
	uint16_t height  = 0;

	core::error last_error = core::error::none;
	bool loaded = false;

public:
	// constructor's
	image() = default;
	image(
		std::string const& image_path, 
		std::string const& image_name, 
		bool load_image_automatically = false
	);

	// destructor
	~image();

	// functions
	bool is_loaded();
	core::error get_last_error();

	core::error load_image();
	const uint8_t* buffer() const;

	const std::string* get_name() const;
	const std::string* get_path() const;
	uint16_t get_width() const;
	uint16_t get_height() const;

	core::error free_buffer();

};

#endif

#endif
