#pragma once

#ifndef CORE_EVENT_HPP
#define CORE_EVENT_HPP

#include "core/types.hpp"

namespace core {

	enum class event_category : u16 {
		unkown = 0,

		mouse,
		keyboard,
		controller,

		window,
		gui,
		graphics,
		physics,
		animation,
		network,

		file,
		memory,

	}; // enum event-category end


	struct event_info {
		u16 category;
		u16 type;
	};

	template<typename type> struct event {
		core::event_info info;
		const type       data;
	};

}

#endif
