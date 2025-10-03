#pragma once

#ifndef IMAGE_CPP
#define IMAGE_CPP

/*
	todo: rework
*/
#if 0

#define  STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

#include "image.hpp"

image::image(
	std::string const& image_path, 
	std::string const& image_name, 
	bool load_image_automatically
){
	this->name = image_name;
	this->path = image_path;

	if (load_image_automatically) {
		this->load_image();
	}

}

/*
	class functions
*/

bool image::is_loaded() {
	return this->loaded;
}

core::error image::get_last_error() {
	return this->last_error;
}

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

core::error image::free_buffer(){

	if (this->data == nullptr) {
		this->loaded = false;
		return core::error::nullptr_image;
	}

	stbi_image_free(this->data);
	
	this->data   = nullptr;
	this->loaded = false;

	return core::error::NO_ERR;
}

core::error image::load_image() {
	int x = 0, y = 0, ch = 0;

	this->data = stbi_load(this->path.c_str(), &x, &y, &ch, 0);

	if (this->data == nullptr) {
		this->loaded = false;
		this->last_error = core::error::FAILED_TO_LOAD_IMAGE;
		return core::error::FAILED_TO_LOAD_IMAGE;
	}

	this->loaded  = true;
	this->width   = uint16_t(x);
	this->height  = uint16_t(y);
	this->channle = ch;
	this->last_error = core::error::NO_ERR;

	return core::error::NO_ERR;
}

// destructor
image::~image() {

	if (this->data != nullptr) {
		stbi_image_free(this->data);
		this->loaded = false;
		this->data = nullptr;
	}
}

#endif

#endif
