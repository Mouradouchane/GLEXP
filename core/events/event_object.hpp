#pragma once

#ifndef CORE_EVENT_HPP
#define CORE_EVENT_HPP

namespace core {
	
	struct event_info {
		u16 type;
		u16 category;
	}

	template<typename type> struct event {
		core::event_info info;
		const type       data;
	}

}

#endif
