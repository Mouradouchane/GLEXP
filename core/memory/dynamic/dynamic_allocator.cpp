#pragma once 

#ifndef CORE_MEMORY_ALLOCATOR_CPP
#define CORE_MEMORY_ALLOCATOR_CPP

#include "core/logger/logger.hpp"
#include "core/locks/scope_lock/scope_lock.hpp"

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

dynamic_allocator::dynamic_allocator(core::dynamic_allocator_configs const& parameters) NOEXP {
    
    // check block size if valid
    if (
        parameters.block_size < dynamic_allocator::min_size_allowed || 
        parameters.block_size > dynamic_allocator::max_size_allowed
    ) {
        CORE_FATAL_F(
            CORE_SIZE_OUT_OF_RANGE, parameters.block_size, "core::dynamic_allocator", 
            dynamic_allocator::min_size_allowed, dynamic_allocator::max_size_allowed 
        );
        return;
    }

    // setup variables
#ifdef DEBUG
    this->_tag_  = parameters.tag;
    this->_name_ = parameters.name;
#endif

    this->is_mt   = parameters.is_multi_thread;
    this->_count_ = 12;
    this->_new_block_size_ = parameters.block_size;
    this->_block_max_allocation_ = parameters.block_max_allocation;

    // setup blocks list
    this->create_blocks_list(this->_count_);

    // add first block
    this->add_block(this->_new_block_size_ , this->_block_max_allocation_ , (u8)this->_tag_);
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

two_pointers dynamic_allocator::allocate_tow(
    core::memory_request const& request_1, core::memory_request const& request_2
) NOEXP {
    return two_pointers{ 0 };
}

void dynamic_allocator::deallocate(void* pointer) NOEXP {

}


/*
    todo: implement these functions
*/

u32 dynamic_allocator::blocks_count() NOEXP {
    return 0;
}

u64 dynamic_allocator::size() NOEXP {
    return 0;
}

u64 dynamic_allocator::current_memory_usage() NOEXP {
    return 0;
}

u64 dynamic_allocator::current_memory_usage(core::memory_tag section_tag) NOEXP {
    if ((u8)section_tag < MAX_MEMORY_TAGS) {
        return this->_sections_[(u8)section_tag];
    }
    else return 0;
}

u64 dynamic_allocator::peak_memory_usage() NOEXP {
    return 0;
}


#ifdef DEBUG

    string const& dynamic_allocator::name() NOEXP {
        return this->_name_;
    }

    core::allocator_tag dynamic_allocator::tag()  NOEXP {
        return this->_tag_;
    }

#else 

    INLINE string dynamic_allocator::name() NOEXP {
        return "";
    }

    INLINE core::allocator_tag dynamic_allocator::tag() NOEXP {
        return core::memory_tag::unkown;
    }

#endif


/*
    private helper functions
*/

INLINE void dynamic_allocator::create_blocks_list(u32 blocks_count) NOEXP {
    
    this->_blocks_ = (core::memory_block*)core::memory::allocate(
        core::g_memory_request{ 
            .size = (sizeof(core::memory_block) * blocks_count ) + (sizeof(bool) * blocks_count) ,
            .tag = (u8)core::allocator_tag::memory_system 
        }
    );

    this->_states_   = (bool*)((byte*)this->_blocks_ + (sizeof(core::memory_block) * blocks_count));
    memset(this->_states_, false, sizeof(bool) * blocks_count);

    this->_capacity_ = blocks_count;
    this->_count_    = 0;

}


INLINE void dynamic_allocator::resize_blocks_list(u32 added_count) NOEXP {
    core::atomic_scope_lock scope_lock(this->_lock_);

    u32 old_capcity = this->_capacity_.load(MEMORY_ORDER_RELAXE);
    u32 old_size    = sizeof(core::memory_block) * old_capcity;

    u32 new_capacity = old_capcity + (added_count ? added_count : old_capcity);
    u32 new_size = (sizeof(core::memory_block) * new_capacity) + (sizeof(bool) * new_capacity);
     
    // allocate new list
    core::memory_block* new_list = (core::memory_block*)core::memory::allocate(
        core::g_memory_request {
            .size = new_size, 
            .tag = (u8)core::allocator_tag::memory_system
        }
    );

    bool* p_states = (bool*)((byte*)new_list + (sizeof(core::memory_block) * new_capacity));

    // copy to new list's
    std::memcpy(new_list, this->_blocks_, old_size);
    std::memcpy(p_states, this->_states_, sizeof(bool) * old_capcity); // move current states to new array
    memset(this->_states_ + old_capcity, false, sizeof(bool)* (new_capacity - old_capcity)); // zero the rest

    // remove old list's
    core::memory::deallocate(this->_blocks_);

    // update to new list and capacity
    this->_blocks_ = new_list;
    this->_states_ = p_states;

    this->_capacity_.store(new_capacity, MEMORY_ORDER_ACQUIRE);

}

INLINE void dynamic_allocator::deallocate_blocks_list() NOEXP {
    core::atomic_scope_lock scope_lock(this->_lock_);

    if (this->_blocks_) { 

        // destruct all the blocks
        for (u32 i = 0; i < this->_count_; i++) {
            (this->_blocks_ + i)->~memory_block();
        }

        // reset variables 
        this->_capacity_ = 0;
        this->_count_    = 0;

        this->_size_ = 0;
        this->_peak_ = 0;
        this->_min_  = 0xFFFFFFFF;

        // deallocate blocks list
        core::memory::deallocate(this->_blocks_);

        this->_blocks_ = nullptr;
        this->_states_ = nullptr;
    }

#ifdef DEBUG
    memset(this->_sections_, 0, sizeof(u8) * MAX_MEMORY_TAGS);
#endif

}

INLINE void dynamic_allocator::add_block(u32 block_size , u16 block_max_allocation , u8 block_tag) NOEXP {
    core::atomic_scope_lock scope_lock(this->_lock_);

    if (this->_count_ < this->_capacity_) {

        if ( ! this->_states_[this->_count_]) {
            this->_states_[this->_count_] = true;
            new (this->_blocks_ + this->_count_) core::memory_block(block_size, block_max_allocation, block_tag);

            this->_count_ += 1;
            this->_size_  += block_size;
            return;
        }
        else { // there is a empty spot but it's not at _count_
            
            // O(N) search for that block
            for (u32 i = 0; i < this->_capacity_; i++) {
                if ( ! this->_states_[i]) {
                    this->_states_[i] = true;
                    new (this->_blocks_ + i) core::memory_block(block_size, block_max_allocation, block_tag);

                    this->_count_ += 1;
                    this->_size_ += block_size;
                    return;
                }
            }

        }

    }
    // else : no empty spot found for new block so resize is needed 
    this->resize_blocks_list(u32(this->_capacity_  * 0.5));
    
    // add new block after resize
    new (this->_blocks_ + this->_count_) core::memory_block(block_size, block_max_allocation, block_tag);
    this->_states_[this->_count_] = true;
    this->_count_ += 1;
    this->_size_  += block_size;
}

INLINE void dynamic_allocator::remove_block(u32 index) NOEXP {
    core::atomic_scope_lock scope_lock(this->_lock_);

    if (index < this->_capacity_){

        if (this->_states_[index]) {
            this->_size_ -= (this->_blocks_ + index)->size();
            
            (this->_blocks_ + index)->~memory_block();
            this->_states_[index] = false;
            this->_count_ -= 1;
        }
    }

}


} // namespace core end


#endif