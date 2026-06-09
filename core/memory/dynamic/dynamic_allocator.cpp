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

static INLINE u64 internal_get_stats(string const& _heap_name_, mi_heap_t* _heap_, stats_type target_stats) NOEXP;


namespace core {

/*
	constructor's
*/

dynamic_allocator::dynamic_allocator(core::allocator_parameters const& parameters) NOEXP {

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
 
}

void* dynamic_allocator::allocate(u32 size , u8 tag) NOEXP {
 
}

tow_pointers dynamic_allocator::allocate_tow(
    core::memory_request const& request_1, core::memory_request const& request_2
) NOEXP {

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

}

u64 dynamic_allocator::current_memory_usage() NOEXP {

}

u64 dynamic_allocator::peak_memory_usage() NOEXP {

}


#ifdef DEBUG

    const string& dynamic_allocator::name() NOEXP {
        return this->_name_;
    }

    core::memory_tag dynamic_allocator::tag()  NOEXP {
        return this->_tag_;
    }

#else 

    INLINE const string& dynamic_allocator::name() NOEXP {
        return "";
    }

    INLINE core::memory_tag dynamic_allocator::tag() NOEXP {
        return core::memory_tag::unkown;
    }

#endif

} // namespace core end


static INLINE u64 internal_get_stats(string const& _heap_name_, mi_heap_t* _heap_, stats_type target_stats) NOEXP {
    
}


#endif