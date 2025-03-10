#pragma once

#ifndef IMAGE_CPP
#define IMAGE_CPP

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "image.hpp"

image::image(std::string const& image_path, std::string const& image_name, bool load_image_automatically) {

	if (!load_image_automatically) {
		this->name = image_name;
		this->path = image_path;
	}
	else {

		int x = 0, y = 0, ch = 0;
		this->data = stbi_load(image_path.c_str(), &x, &y, &ch, 0);

		if (this->data == nullptr) {
			this->last_error = IMAGE_ERROR::FAILED_TO_LOAD_IMAGE;
			return;
		}

		this->width = uint16_t(x);
		this->height = uint16_t(y);
		this->channle = ch;

		this->loaded = true;
		this->last_error = IMAGE_ERROR::NO_ERR;
	}

}

/*
	class functions
*/

const uint8_t* image::buffer() const{
	return this->data;
}

const std::string* image::get_name() const{
	return &this->name;
}

const std::string* image::get_path() const{
	return &this->path;
}

uint16_t image::get_width() const{
	return this->width;
}

uint16_t image::get_height() const{
	return this->height;
}

IMAGE_ERROR image::free_buffer(){

	if (this->data == nullptr) {
		this->loaded = false;
		return IMAGE_ERROR::NULLPTR_IMAGE;
	}

	stbi_image_free(this->data);
	
	this->data   = nullptr;
	this->loaded = false;

	return IMAGE_ERROR::NO_ERR;
}

IMAGE_ERROR image::resource() {
	int x = 0, y = 0, ch = 0;

	this->data = stbi_load(this->path.c_str(), &x, &y, &ch, 0);

	if (this->data == nullptr) {
		this->loaded = false;
		this->last_error = IMAGE_ERROR::FAILED_TO_LOAD_IMAGE;
		return IMAGE_ERROR::FAILED_TO_LOAD_IMAGE;
	}

	this->loaded  = true;
	this->width   = uint16_t(x);
	this->height  = uint16_t(y);
	this->channle = ch;
	this->last_error = IMAGE_ERROR::NO_ERR;

	return IMAGE_ERROR::NO_ERR;
}

// destructor
image::~image() {

	if (this->data != nullptr) {
		stbi_image_free(this->data);
		this->data = nullptr;
	}
}

#endif
