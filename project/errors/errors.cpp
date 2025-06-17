#pragma once

#ifndef ERRORS_CPP
#define ERRORS_CPP

#define _WIN32
#include <thread>
#include "boxer/boxer.h" // https://github.com/aaronmjacobs/Boxer
#include "errors.hpp"

#ifdef DEBUG

void display_error_messagebox(
	std::string const& message,
	std::string const& function,
	std::string const& file,
	uint32_t line
) {
	// NOTE: show function will stop current thread from execution
	std::string full_message = ("error at " + file) + (" line " + line) + (" from " + function) + (" !\n" + message);
	boxer::show(
		full_message.c_str(),
		"ERROR",
		boxer::Style::Error,
		boxer::Buttons::OK
	);

}

void display_warn_messagebox(
	std::string const& message,
	std::string const& function,
	std::string const& file,
	uint32_t line
) {

	std::thread warning_thread(
		[&]() {
			std::string full_message = ("warning from " + file) + (" line " + line) + (" : " + function) + (" !\n" + message);
			
			boxer::show(
				full_message.c_str(), "WARNING", boxer::Style::Warning, boxer::Buttons::OK
			);
		}
	);

}

#else 

void display_error_messagebox(std::string const& message) {

	// NOTE: show function will stop current thread from execution
	boxer::show(
		message.c_str(),"ERROR", boxer::Style::Error, boxer::Buttons::OKCancel
	);
}

void display_warn_messagebox(std::string const& message) {

	std::thread warning_thread(
		[&]() {
			boxer::show(
				message.c_str(), "WARNING", boxer::Style::Warning, boxer::Buttons::OK
			);
		}
	);

}

#endif


#endif