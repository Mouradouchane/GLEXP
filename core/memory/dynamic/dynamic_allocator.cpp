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
    
    // check memory budget
    if (parameters.memory_budget < parameters.blocks_size) {
    #ifdef DEBUG
        CORE_WARN_F(
            "core::dynamic_allocator(): bad config 'memory_budget={}bytes' is smaller than 'blocks_size={}bytes' !",
            parameters.memory_budget, parameters.blocks_size
        );
        CORE_INFO(
            "core::dynamic_allocator(): auto reconfig to memory_budget from {}bytes up to {}bytes .",
            parameters.memory_budget, parameters.blocks_size
        );
    #endif

         this->_memory_budget_ = parameters.blocks_size;
    }
    else this->_memory_budget_ = parameters.memory_budget;

    // check blocks size
    if (
        parameters.blocks_size < dynamic_allocator::min_size_allowed ||
        parameters.blocks_size > dynamic_allocator::max_size_allowed
    ) {
    #ifdef DEBUG
        CORE_WARN(CORE_LOG_CONFIG_ALL,
            "core::dynamic_allocator(): bad config blocks size '{}bytes' !" CORE_WARNINIG_RUNTIME_CRASH,
            parameters.blocks_size
        );

        if (parameters.blocks_size < dynamic_allocator::min_size_allowed) {
            this->_blocks_size_ = dynamic_allocator::min_size_allowed;

            CORE_INFO(
                "core::dynamic_allocator(): blocks size upsized to {}bytes per block for DEBUG_ONLY !",
                this->_blocks_size_
            );
        }
        else {
            this->_blocks_size_ = dynamic_allocator::max_size_allowed;

            CORE_INFO(
                "core::dynamic_allocator(): blocks size downsized to {}bytes per block for DEBUG_ONLY !",
                this->_blocks_size_
            );
        }

    #else 
        if (parameters.blocks_size < dynamic_allocator::min_size_allowed) {
            this->_blocks_size_ = dynamic_allocator::min_size_allowed;
        }
        else {
            this->_blocks_size_ = dynamic_allocator::max_size_allowed;
        }
    #endif

    }
    else {
        this->_blocks_size_ = parameters.blocks_size;
    }

    // check max allocations
    if (parameters.max_allocations_per_block > MAX_ALLOCATIONS_PRE_BLOCK) {
    #ifdef DEBUG
        CORE_WARN_F(
            "core::dynamic_allocator(): max allocations per block '{}' is higher than the maximum allowed '{}' !",
            parameters.max_allocations_per_block , MAX_ALLOCATIONS_PRE_BLOCK
        );
        CORE_INFO(
            "core::dynamic_allocator(): auto reconfig max allocations per block from {} to {} ." ,
            parameters.max_allocations_per_block, MAX_ALLOCATIONS_PRE_BLOCK
        );
    #endif

        this->_blocks_max_allocations_  = MAX_ALLOCATIONS_PRE_BLOCK;
    }
    else this->_blocks_max_allocations_ = parameters.max_allocations_per_block | 1;
    
    // setup other variables
#ifdef DEBUG
    this->_tag_  = parameters.tag;
    this->_name_ = parameters.name;
#endif

    this->_is_mt_ = parameters.is_multi_thread;

    // add first block
    this->add_new_block(this->_blocks_size_);
}

/*
	destructor
*/

dynamic_allocator::~dynamic_allocator() NOEXP {

    // note: the blocks destructor will get called automatically because _block_[]
    /*
    for (u32 i = 0; i < this->_capacity_; i++) {
        
        if (this->_blocks_status_[i]) {
            // note: memory_block auto detected "memory leaks"
            this->_blocks_[i].~memory_block();
            this->_blocks_status_[i] = false;
        }

    }
    */

    this->_size_ = 0;
    this->_blocks_count_  = 0;
    this->_memory_budget_ = 0;
}

/*
    public functions
*/

core::memory_handle dynamic_allocator::allocate(core::memory_request const& request) NOEXP {
    
    core::memory_handle handle;

    // loop over all blocks
    for (u8 i = 0; i < this->_blocks_count_; i++) {
    
        // if block is alive
        if (this->_blocks_status_[i]) {

            // find a "not-busy" block
            if ( ! this->_blocks_[i].is_busy()) {

                // try allocate , block is self-locking
                handle = this->_blocks_[i].allocate(request);

                // if success
                if (handle.response == core::allocator_response::success) {
                    handle.block_index = i;
                    this->update_size_variables(request, handle, true);

                    return handle;
                }
            }

        }

    }
    /*
        else mean all the block is busy at the moment or full
    */
    
    // try allocate new block if possible
    u8 index = this->add_new_block(request.size);

    // try to allocate
    if (index < this->_capacity_) {
        handle = this->_blocks_[index].allocate(request);
        handle.block_index = index;

        this->update_size_variables(request, handle, true);

        return handle;
    }

    // failed to find new block or memory
    return core::memory_handle { 
            .response = core::allocator_response::full,
            .ptr = nullptr,
    };

}

INLINE core::memory_handle dynamic_allocator::allocate(u32 size , u8 tag) NOEXP {
    return this->allocate(
        core::memory_request{ 
            .size = size , 
            .alignement = 0,
            .tag = tag , 
        }
    );
}

INLINE core::memory_handle dynamic_allocator::allocate(u32 size, u16 alignement, u8 tag) NOEXP {
    return this->allocate(
        core::memory_request{
            .size = size ,
            .alignement = alignement,
            .tag = tag
        }
    );
}

core::memory_handle_2 dynamic_allocator::allocate_tow(
    core::memory_request const& request_1, core::memory_request const& request_2
) NOEXP {
    
    core::memory_handle_2 handle;

    for (u8 i = 0; i < this->_blocks_count_; i++) {

        // if block is alive
        if (this->_blocks_status_[i]) {

            // if block not busy
            if (! this->_blocks_[i].is_busy()) {

                // try allocate
                handle = this->_blocks_[i].allocate_tow(request_1 , request_2);

                // if success
                if (
                    (handle.handle_1.response == core::allocator_response::success) && 
                    (handle.handle_2.response == core::allocator_response::success)
                ) {
                    handle.handle_1.block_index = i;
                    handle.handle_2.block_index = i;

                    this->update_size_variables(request_1, handle.handle_1, true);
                    this->update_size_variables(request_2, handle.handle_2, true);

                    return handle;
                }
            }

        }

    }
    /*
        else mean all the block is busy at the moment or full
    */

    // try allocate new block if possible
    u8 index = this->add_new_block(request_1.size + request_2.size);

    // "second attempt" : try to allocate
    if (index < this->_capacity_) {
        handle = this->_blocks_[index].allocate_tow(request_1, request_2);
        handle.handle_1.block_index = index;
        handle.handle_2.block_index = index;
        
        this->update_size_variables(request_1, handle.handle_1, true);
        this->update_size_variables(request_2, handle.handle_2, true);

        return handle;
    }

    // failed to find new block or memory
    return core::memory_handle_2{
        core::memory_handle {.response = core::allocator_response::full},
        core::memory_handle {.response = core::allocator_response::full}
    };

}

void dynamic_allocator::deallocate(core::memory_handle handle) NOEXP {

    if (handle.block_index >= this->_capacity_) {
        #ifdef DEBUG
            CORE_ERROR_F(CORE_INDEX_OUT_OF_RANGE , handle.block_index , "core::dynamic_allocator");
            DEBUG_BREAK;
        #endif
        return;
    }

    core::memory_allocation alloc = this->_blocks_[handle.block_index].get_allocation_info(handle);

    if (! this->_blocks_[handle.block_index].deallocate(handle)) {
        CORE_WARN_F(
            "core::dynamic_allocator.deallocate(): memory block failed to deallocate {} !",
            core::pointer_to_hex_string(handle.ptr)
        );

        return;
    }

    this->update_size_variables(core::memory_request{.size = alloc.size , .tag = alloc.tag }, handle , false);

}


/*
    todo: implement these functions
*/

u32 dynamic_allocator::blocks_count() NOEXP {
    return this->_blocks_count_;
}

u64 dynamic_allocator::size() NOEXP {
    return this->_size_.load(MEMORY_ORDER_RELAXE);
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
    return this->_peak_;
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
    note: call this function only from allocate/deallocate
*/
INLINE void core::dynamic_allocator::update_size_variables (
    core::memory_request const& request, core::memory_handle const& handle, bool increment
) NOEXP {

    // update allocator size
    if (increment) this->_size_.fetch_add(request.size, MEMORY_ORDER_ACQUIRE);
    else this->_size_.fetch_sub(request.size, MEMORY_ORDER_ACQUIRE);

#ifdef DEBUG
    // update section size
    if (request.tag < MAX_MEMORY_TAGS) {
        if(increment) this->_sections_[request.tag].fetch_add(request.size , MEMORY_ORDER_ACQUIRE);
        else this->_sections_[request.tag].fetch_sub(request.size, MEMORY_ORDER_ACQUIRE);
    }

    // update _min_ & _peak_
    core::atomic_scope_lock scope_lock(this->_lock_);

    u32 min  = this->_min_;
    u32 peak = this->_peak_;

    this->_min_  = (request.size < min)  ? request.size : min;
    this->_peak_ = (request.size > peak) ? request.size : peak;
#endif

}

/*
    private helper functions
*/

INLINE u8 dynamic_allocator::add_new_block(u32 target_size) NOEXP{
    
    core::atomic_scope_lock scope_lock(this->_lock_);

    u32 block_size;

    // check size
    if (target_size > this->_blocks_size_)  block_size = target_size;
    if (target_size <= this->_blocks_size_) block_size = this->_blocks_size_;
    
    // check allocator memory budget
    if (this->_memory_budget_ < (this->_size_.load(MEMORY_ORDER_RELAXE) + block_size)) {
        #ifdef DEBUG
            CORE_WARN(
                CORE_LOG_CONFIG_ALL , CORE_WARNING_OUT_OF_BUDGET  CORE_WARNINIG_RUNTIME_CRASH,
                "core::dynamic_allocator" , this->_memory_budget_
            );
        #else
            CORE_FATAL(
                CORE_LOG_CONFIG_ALL, CORE_WARNING_OUT_OF_BUDGET,
                "core::dynamic_allocator", this->_memory_budget_
            );
        #endif

        return this->_capacity_;
    }

    if (this->_blocks_count_ < this->_capacity_) {
        if (this->_blocks_status_[this->_blocks_count_] == false) {
            u8 index = this->_blocks_count_;

            new (this->_blocks_ + index) core::memory_block(block_size, this->_blocks_max_allocations_ , (u8)this->_tag_);

            this->_blocks_status_[index] = true;
            this->_size_ += block_size;
            this->_blocks_count_ += 1;

            // return new block index
            return index;
        }
        else {
            CORE_FATAL(CORE_LOG_CONFIG_ALL,
                "core::dynamic_allocator: failed to find empty spot for new block ! this could be a bug , count={} , capacity={}",
                this->_blocks_count_ , this->_capacity_
            );
            return this->_capacity_;
        } 
    }
    else return this->_capacity_;
}

/*
INLINE void dynamic_allocator::remove_block(u8 index) NOEXP {

    if (index < this->_capacity_){

        if (this->_blocks_status_[index]) {
            this->_blocks_status_[index] = false;

            this->_size_ -= (this->_blocks_ + index)->size();
            
            (this->_blocks_ + index)->~memory_block();
            this->_blocks_count_ -= 1;
        }
    }

}
*/

} // namespace core end


#endif