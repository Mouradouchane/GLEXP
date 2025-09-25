#pragma once

#ifndef ERRORS_CPP
#define ERRORS_CPP

#include "core/macros.hpp"
#include <thread>

#include "libs/boxer/boxer.h" // project repo : https://github.com/aaronmjacobs/Boxer
#ifdef WINDOWS
	#include "libs/boxer/boxer_win.cpp" 
#elif LINUX
	#include "libs/boxer/boxer_linux.cpp"
#else
	#error "boxer library can't compile for this system !"
#endif

#include "errors.hpp"

#ifdef DEBUG

void display_error_messagebox(
	std::string const& message,
	std::string const& function,
	std::string const& file,
	uint32_t line
) {
	// NOTE: show function will stop current thread from execution
	std::string full_message = ("file: " + file + "\n") + ("line: " + std::to_string(line) + "\n") + ("code: " + function + "\n") + ("message: " + message);
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
			std::string full_message = ("file: " + file + "\n") + ("line: " + std::to_string(line) + "\n") + ("code: " + function + "\n") + ("message: " + message);
			
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