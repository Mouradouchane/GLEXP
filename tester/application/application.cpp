#pragma once 

#ifndef APPLICATION_CPP
#define APPLICATION_CPP

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include <iostream>

#include "application.hpp"

using namespace ftxui;

namespace tester {

	bool running = true;

	Element document;
	Screen screen = Screen(800,600);

	void init() {
		// setup ui 

		document = hbox({
			text("left")   | border,
			text("middle") | border | flex,
			text("right")  | border,
		});

		screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));

		// setup tests

	}

	void run() {
		
		// render 
		Render(screen, document);
		screen.Print();

		while (running) {
			// maybe take inputs

			// maybe run tests


			// sleep
		}

	}

	void shutdown() {
		// log results to file 

	}

}

#endif // !APPLICATION_HPP
