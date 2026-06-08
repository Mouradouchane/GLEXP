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

namespace core {

/*
	constructor's
*/

dynamic_allocator::dynamic_allocator(core::allocator_parameters const& parameters) NOEXP {

    if (parameters.size < dynamic_allocator::min_size_allowed || parameters.size > dynamic_allocator::max_size_allowed) {
        CORE_FATAL(
            CORE_LOG_CONFIG_ALL, ALLOCATOR_SIZE_NOT_ALLOWED,
            parameters.size , parameters.name,
            dynamic_allocator::min_size_allowed, dynamic_allocator::max_size_allowed
        );

        return;
    }

	// force mi-malloc to pre-allocate memory and don't give it back to the OS automatically
	mi_option_set(mi_option_arena_eager_commit, 1); // pre-allocate ram
	mi_option_set(mi_option_disallow_os_alloc, 1);  // crash if failed to allocate memory
	mi_option_set(mi_option_purge_delay, -1);       // don't auto return memory block to the OS

    // reserve and back the entire flat block from the OS right now
    int result = mi_reserve_os_memory_ex(
        parameters.size ,
        true,   // commit immediately
        true,   // allow large/huge pages for optimal TLB cache performance
        true,   // EXCLUSIVE: locks this block so global mi_malloc can't steal it !
        &this->_arena_id_
    );

    // if failed
    if (result != 0) {
        CORE_FATAL(CORE_LOG_CONFIG_ALL, FAILED_TO_CREATE_ALLOCATOR , parameters.name, parameters.size);
        return;
    }

    // create the thread-safe heap interface tied strictly into this pre-allocated arena
    this->_heap_ = mi_heap_new_in_arena(this->_arena_id_);
   
    if (!this->_heap_) {
        CORE_FATAL(CORE_LOG_CONFIG_ALL, FAILED_TO_CREATE_ALLOCATOR, parameters.name, parameters.size);
        return;
    }

#ifdef DEBUG
    this->_name_ = *parameters.name;
    this->_tag_  = parameters.tag;
#endif

    // todo: work on multi-threading asap
    this->_multi_threaded_ = parameters.multi_threaded;

}

/*
	destructor
*/

dynamic_allocator::~dynamic_allocator() NOEXP {

    if (this->_heap_) {
        mi_heap_delete(this->_heap_); // Safely purges internal management trackers
        this->_heap_ = nullptr;
    }

}

/*
    public functions
*/

void* dynamic_allocator::allocate(core::memory_request const& request) NOEXP {
 
#ifdef DEBUG
    void* ptr = mi_heap_malloc_aligned(this->_heap_, request.size + 1 , request.alignement);

    if (!ptr) {
        CORE_FATAL(CORE_LOG_CONFIG_ALL, ALLOCATOR_FAILED, this->_name_, request.size, core::to_string(this->_tag_,request.tag));
        return;
    }
    
    u8* tptr = (u8*)ptr + request.size;
       *tptr = (u8)request.tag;

    this->sections_sizes[request.tag] += request.size;
    
    return ptr;

#else
    void*  ptr = mi_heap_malloc_aligned(this->_heap_, request.size, request.alignement);
    return ptr;
#endif

}

void* dynamic_allocator::allocate(u32 size , u8 tag) NOEXP {
 
#ifdef DEBUG
    void* ptr = mi_heap_malloc(this->_heap_, size + 1);

    if (!ptr) {
        CORE_FATAL(CORE_LOG_CONFIG_ALL, ALLOCATOR_FAILED, this->_name_, size, core::to_string(this->_tag_, tag));
        return;
    }

    u8* tptr = (u8*)ptr + size;
       *tptr = (u8)tag;

    this->sections_sizes[tag] += size;
#else
    void* ptr = mi_heap_malloc(this->_heap_, size);
#endif

}

tow_pointers dynamic_allocator::allocate_tow(
    core::memory_request const& request_1, core::memory_request const& request_2
) NOEXP {

#ifdef DEBUG

    // allocate memory
    void* ptr1 = mi_heap_malloc_aligned(this->_heap_, request_1.size + 1, request_1.alignement);
    void* ptr2 = mi_heap_malloc_aligned(this->_heap_, request_2.size + 1, request_2.alignement);

    // tag the memory
    if (ptr1 && ptr2) {
        u8* tptr1 = ((u8*)ptr1 + request_1.size);
           *tptr1 = (u8)request_1.tag;

        u8* tptr2 = ((u8*)ptr2 + request_2.size);
           *tptr2 = (u8)request_2.tag;

        // update list
        this->sections_sizes[(u8)request_1.tag] += request_1.size;
        this->sections_sizes[(u8)request_2.tag] += request_2.size;

        return tow_pointers{ ptr1 , ptr2 };
    }
    else {
        CORE_FATAL_F(ALLOCATOR_FAILED_2 , this->_name_ , request_1.size + request_2.size , request_1.tag , request_2.tag);
        return tow_pointers{ nullptr , nullptr };
    }

#else
    return tow_pointers{
        mi_heap_malloc_aligned(this->_heap_, request_1.size, request_1.alignement) ,
        mi_heap_malloc_aligned(this->_heap_, request_2.size, request_2.alignement)
    };

#endif

}

void dynamic_allocator::deallocate(void* pointer) NOEXP {

#ifdef DEBUG
    u32 size = mi_usable_size(pointer);
    u8  tag = *((u8*)pointer + (size - 1));
    this->sections_sizes[tag] -= size;
#endif

    mi_free(pointer);

}

} // namespace core end

#endif