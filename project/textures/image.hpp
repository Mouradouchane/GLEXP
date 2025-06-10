#pragma once

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "errors.hpp"
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

	ERR  last_error = ERR::NO_ERR;
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
	ERR get_last_error();

	ERR load_image();
	const uint8_t* buffer() const;

	const std::string* get_name() const;
	const std::string* get_path() const;
	uint16_t get_width() const;
	uint16_t get_height() const;

	ERR free_buffer();

};

#endif
