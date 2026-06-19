#pragma once 

#ifndef CORE_MEMORY_ALLOCATOR_CPP
#define CORE_MEMORY_ALLOCATOR_CPP

#include "core/logger/logger.hpp"

#include "dynamic_allocator.hpp"

#ifdef DEBUG
    static auto _core_dynamic_alloc_logger_ = CORE_GET_LOGGER(MEMORY_ALLOCATOR_LOGGER);
#else 
    static auto _core_dynamic_alloc_logger_ = nullptr;
#endif

#define _LOGGER_  _core_dynamic_alloc_logger_

static enum stats_type : u8 {
    total,
    peak,
    current,
};

namespace core {

/*
	constructor's
*/

dynamic_allocator::dynamic_allocator(core::g_memory_request const& request) NOEXP {

}

/*
	destructor
*/

dynamic_allocator::~dynamic_allocator() NOEXP {

  
}

/*
    public functions
*/

void* dynamic_allocator::allocate(core::memory_request const& request) NOEXP {
    return nullptr;
}

void* dynamic_allocator::allocate(u32 size , u8 tag) NOEXP {
    return nullptr;
}

tow_pointers dynamic_allocator::allocate_tow(
    core::memory_request const& request_1, core::memory_request const& request_2
) NOEXP {
    return tow_pointers{ 0 };
}

void dynamic_allocator::deallocate(void* pointer) NOEXP {

}

u64 dynamic_allocator::current_memory_usage(u8 section_tag) NOEXP {

    if (section_tag < this->sections_sizes.size()) {
        return this->sections_sizes[section_tag];
    }
    else return 0;
}

u64 dynamic_allocator::total_memory() NOEXP {
    return 0;
}

u64 dynamic_allocator::current_memory_usage() NOEXP {
    return 0;
}

u64 dynamic_allocator::peak_memory_usage() NOEXP {
    return 0;
}


#ifdef DEBUG

    string const& dynamic_allocator::get_name() NOEXP {
        return this->_allocator_name_;
    }

    core::memory_tag dynamic_allocator::get_tag()  NOEXP {
        return this->_allocator_tag_;
    }

#else 

    INLINE string dynamic_allocator::get_name() NOEXP {
        return "";
    }

    INLINE core::memory_tag dynamic_allocator::get_tag() NOEXP {
        return core::memory_tag::unkown;
    }

#endif

} // namespace core end


#endif