#if 1
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
    if (
        parameters.memory_budget < core::dynamic_allocator::min_size_allowed || 
        parameters.memory_budget > core::dynamic_allocator::max_size_allowed
    ) {

        CORE_WARN_F(
            "core::dynamic_allocator(): memory budget {}bytes not allowed , min={} , max={}!",
            parameters.memory_budget, core::dynamic_allocator::min_size_allowed , core::dynamic_allocator::max_size_allowed
        );

        return;
    }

    this->_memory_budget_ = parameters.memory_budget;
    this->_blocks_size_ = u64(parameters.memory_budget / this->_capacity_);

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

    if (parameters.allocate_all_at_once) {
        for (u32 i = 0; i < this->_capacity_; i++) {
            this->add_new_block(this->_blocks_size_);
        }
    }
    else {
        // add first block
        this->add_new_block(this->_blocks_size_);
    }
}

/*
	destructor
*/

dynamic_allocator::~dynamic_allocator() NOEXP {

    // note: all the blocks "destructor" will get called automatically because _block_ is array[]
 
    this->_size_ = 0;
    this->_blocks_count_  = 0;
    this->_memory_budget_ = 0;

    CORE_DEBUG(0, "core::dynamic_allocator {} destructed !" , core::pointer_to_hex_string(this) );
}

/*
    public functions
*/

memory_handle dynamic_allocator::allocate(memory_request request) NOEXP {
    
    switch (this->_is_mt_) {
        case true  : { return this->allocate_on_mt(request); } break; //  multi-thread allocation
        case false : { return this->allocate_on_st(request); } break; // single-thread allocation
    }

    // todo: move this code to mt/st functions
    /*
    memory_handle handle;

    // loop over all blocks
    for (u8 i = 0; i < this->_blocks_count_; i++) {
    
        // if block is alive
        if (this->_blocks_status_[i]) {

            // find a "not-busy" block
            if ( ! this->_blocks_[i].is_busy()) {

                // try allocate , block is self-locking
                handle = this->_blocks_[i].allocate(request);

                // if success
                if (handle.response == allocator_response::success) {
                    handle._block_index_ = i;
                    this->update_size_variables(request, handle, true);

                    return handle;
                }
            }

        }

    }
    */
    /*
        else mean all the block is busy at the moment or full
    */
    /*
    // try allocate new block if possible
    u8 index = this->add_new_block(request.size);

    // try to allocate
    if (index < this->_capacity_) {
        handle = this->_blocks_[index].allocate(request);
        handle._block_index_ = index;

        this->update_size_variables(request, handle, true);

        return handle;
    }

    // failed to find new block or memory
    return memory_handle{ };
    */

}

memory_handle dynamic_allocator::allocate(u32 size , memory_tag tag) NOEXP {

    switch (this->_is_mt_) {

        case true  : { //  multi-thread allocation
            return this->allocate_on_mt(memory_request{ .size = size, .alignement = 0, .tag = tag,}); 
        } break; 

        case false : { // single-thread allocation
            return this->allocate_on_st(memory_request{ .size = size, .alignement = 0, .tag = tag, }); 
        } break; 

    }

}

memory_handle dynamic_allocator::allocate(u32 size, u16 alignement, memory_tag tag) NOEXP {

    switch (this->_is_mt_) {

        case true: { //  multi-thread allocation
            return this->allocate_on_mt(memory_request{ .size = size, .alignement = alignement, .tag = tag, });
        } break;

        case false: { // single-thread allocation
            return this->allocate_on_st(memory_request{ .size = size, .alignement = alignement, .tag = tag, });
        } break;

    }

}

memory_handle_2 dynamic_allocator::allocate_tow(
    memory_request const& request_1, memory_request const& request_2
) NOEXP {
    
    memory_handle_2 handle;

    for (u8 i = 0; i < this->_blocks_count_; i++) {

        // if block is alive
        if (this->_blocks_status_[i]) {

            // if block not busy
            if (! this->_blocks_[i].is_busy()) {

                // try allocate
                handle = this->_blocks_[i].allocate_tow(request_1 , request_2);

                // if success
                if (
                    (handle.handle_1.response == allocator_response::success) && 
                    (handle.handle_2.response == allocator_response::success)
                ) {
                    handle.handle_1._block_index_ = i;
                    handle.handle_2._block_index_ = i;

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
        handle.handle_1._block_index_ = index;
        handle.handle_2._block_index_ = index;
        
        this->update_size_variables(request_1, handle.handle_1, true);
        this->update_size_variables(request_2, handle.handle_2, true);

        return handle;
    }

    // failed to find new block or memory
    return memory_handle_2{
        memory_handle {},
        memory_handle {}
    };

}

void dynamic_allocator::deallocate(memory_handle handle) NOEXP {

    switch (this->_is_mt_) {

        case true: { //  multi-thread allocation
            this->deallocate_on_mt(handle);
        } break;

        case false: { // single-thread allocation
            this->deallocate_on_st(handle);
        } break;

    }

    // todo: move this code to st/mt deallocate
    /*
    if (handle._block_index_ >= this->_capacity_) {
        #ifdef DEBUG
            CORE_ERROR_F(CORE_INDEX_OUT_OF_RANGE , handle._block_index_ , "core::dynamic_allocator");
            DEBUG_BREAK;
        #endif
        return;
    }

    memory_allocation alloc = this->_blocks_[handle._block_index_].get_allocation_info(handle);
    
    if (! this->_blocks_[handle._block_index_].deallocate(handle)) {
        CORE_WARN_F(
            "core::dynamic_allocator.deallocate(): memory block failed to deallocate {} !",
            core::pointer_to_hex_string(handle.ptr)
        );

        return;
    }

    this->update_size_variables(memory_request{ .size = alloc.size , .tag = (memory_tag)alloc.tag }, handle , false);
    */
}


u32 dynamic_allocator::blocks_count() NOEXP {
    return this->_blocks_count_;
}

u64 dynamic_allocator::size() NOEXP {
    return this->_size_.load(MEMORY_ORDER_RELAXE);
}


/*
    todo: implement these functions
*/
u64 dynamic_allocator::current_memory_usage() NOEXP {
    return 0;
}

u64 dynamic_allocator::current_memory_usage(memory_tag section_tag) NOEXP {
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

subsystem_memory_tag dynamic_allocator::tag()  NOEXP {
    return this->_tag_;
}
#else 
string dynamic_allocator::name() NOEXP {
    return "";
}

subsystem_memory_tag dynamic_allocator::tag() NOEXP {
    return memory_tag::unkown;
}
#endif


/*
    private helper functions
*/


/*
    note: call this function only from allocate/deallocate
*/
INLINE void core::dynamic_allocator::update_size_variables (
    memory_request const& request, memory_handle const& handle, bool increment
) NOEXP {

    // update allocator size
    if (increment) this->_size_.fetch_add(request.size, MEMORY_ORDER_ACQUIRE);
    else this->_size_.fetch_sub(request.size, MEMORY_ORDER_ACQUIRE);

#ifdef DEBUG
    // update section size
    if ((u8)request.tag < MAX_MEMORY_TAGS) {
        if(increment) this->_sections_[(u8)request.tag].fetch_add(request.size , MEMORY_ORDER_ACQUIRE);
        else this->_sections_[(u8)request.tag].fetch_sub(request.size, MEMORY_ORDER_ACQUIRE);
    }

    // update _min_ & _peak_
    core::atomic_scope_lock scope_lock(this->_lock_);

    u32 min  = this->_min_;
    u32 peak = this->_peak_;

    this->_min_  = (request.size < min)  ? request.size : min;
    this->_peak_ = (request.size > peak) ? request.size : peak;
#endif

}


INLINE u8 dynamic_allocator::add_new_block(u32 target_size) NOEXP{
    
    core::atomic_scope_lock scope_lock(this->_lock_);
    u32 block_size;

    // check size
    if (target_size >  this->_blocks_size_) block_size = target_size;
    if (target_size <= this->_blocks_size_) block_size = this->_blocks_size_;
    
    // check allocator memory budget
    if (this->_memory_budget_ < (this->_size_.load(MEMORY_ORDER_RELAXE) + block_size)) {
        #ifdef DEBUG
            CORE_WARN(
                CORE_LOG_CONFIG_ALL , CORE_WARNING_OUT_OF_BUDGET  CORE_WARNINIG_RUNTIME_CRASH,
                "core::dynamic_allocator" , this->_memory_budget_
            );

            DEBUG_BREAK;
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
                this->_blocks_count_.load(MEMORY_ORDER_RELAXE) , this->_capacity_
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

#endif