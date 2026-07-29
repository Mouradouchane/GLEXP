#if 1
#pragma once

#ifndef CORE_TIMER_CPP
#define CORE_TIMER_CPP

#include "timer.hpp"


timer::timer() NOEXP {

	this->start_point = GET_STEADY_TIME();

}

timer::~timer() NOEXP {

}

#endif
#endif 