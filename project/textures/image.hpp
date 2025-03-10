#pragma once

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <string>

enum class IMAGE_ERROR : uint16_t {
	NO_ERR,
	FAILED_TO_LOAD_IMAGE,
	NULLPTR_IMAGE,
};

class image {

private:
	// propertie's
	uint8_t* data = nullptr;
	int channle = -1;

	std::string name = "untitled";
	std::string path = "";

	uint16_t width   = 0; 
	uint16_t height  = 0;

	IMAGE_ERROR last_error = IMAGE_ERROR::NO_ERR;
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
	IMAGE_ERROR resource();
	const uint8_t* buffer() const;

	const std::string* get_name() const;
	const std::string* get_path() const;
	uint16_t get_width() const;
	uint16_t get_height() const;

	IMAGE_ERROR free_buffer();

};

#endif
