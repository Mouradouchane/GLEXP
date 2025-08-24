#include <iostream>
#include "application/application.hpp"

int main() {

	tester::init();
	tester::run();
	tester::shutdown();

	return EXIT_SUCCESS;
}