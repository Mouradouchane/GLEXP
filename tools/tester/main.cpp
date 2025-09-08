#include <iostream>
#include "tools/tester/resource/resource.h"
#include "tools/tester/application/application.hpp"

int main() {

	tester::init();
	tester::run();
	tester::shutdown();

	return EXIT_SUCCESS;
}