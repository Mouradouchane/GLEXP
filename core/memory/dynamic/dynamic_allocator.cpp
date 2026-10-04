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

dynamic_allocator::dynamic_allocator(string const& name, const u64 memory_budget, subsystem_memory_tag tag) NOEXP {
    
    // check memory budget
    if (memory_budget < dynamic_allocator::min_budget_allowed || memory_budget > dynamic_allocator::max_budget_allowed) {

        CORE_WARN_F(
            "core::dynamic_allocator(): memory budget {}bytes not allowed , min={} , max={}!",
            memory_budget, dynamic_allocator::min_budget_allowed , dynamic_allocator::max_budget_allowed
        );

        return;
    }

    _memory_budget_ = memory_budget;
    _budget_ = memory_budget;

    // try to allocate memory budget
    _handle_ = core::memory::allocate(
        g_memory_request{
            .size = memory_budget,
            .tag  = tag
        }
    );

    if (_handle_.response() != allocator_response::success) {
        CORE_FATAL(CORE_LOG_CONFIG_ALL, "dynamic_allocator failed to allocate memory budget {}bytes", memory_budget);
        return;
    }

    // setup memory variables
    _start_ = (byte*)_handle_.ptr;
    _end_   = _start_ + memory_budget;
    _seek_  = _start_;

#ifdef DEBUG
    _tag_  = tag;
    _name_ = name;
#endif

    // add the first block
    add_new_block(_blocks_default_size_);
}

/*
	destructor
*/

dynamic_allocator::~dynamic_allocator() NOEXP {

    core::memory::deallocate(_handle_);
    
    _start_   = nullptr;
    _end_     = nullptr;
    _seek_    = nullptr;

    _budget_        = 0;
    _blocks_count_  = 0;
    _memory_budget_ = 0;

    CORE_DEBUG(0, "core::dynamic_allocator {} destructed !" , core::pointer_to_hex_string(this) );
}

/*
    public functions
*/

memory_handle dynamic_allocator::allocate(memory_request request) NOEXP {
    
    switch (_is_mt_) {
        case true  : { return allocate_on_mt(request); } break; //  multi-thread allocation
        case false : { return allocate_on_st(request); } break; // single-thread allocation
    }

    // todo: move this code to mt/st functions
    /*
    memory_handle handle;

    // loop over all blocks
    for (u8 i = 0; i < _blocks_count_; i++) {
    
        // if block is alive
        if (_blocks_status_[i]) {

            // find a "not-busy" block
            if ( ! _blocks_[i].is_busy()) {

                // try allocate , block is self-locking
                handle = _blocks_[i].allocate(request);

                // if success
                if (handle.response == allocator_response::success) {
                    handle._block_index_ = i;
                    update_size_variables(request, handle, true);

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
    u8 index = add_new_block(request.size);

    // try to allocate
    if (index < _capacity_) {
        handle = _blocks_[index].allocate(request);
        handle._block_index_ = index;

        update_size_variables(request, handle, true);

        return handle;
    }

    // failed to find new block or memory
    return memory_handle{ };
    */

}

memory_handle dynamic_allocator::allocate(u32 size , memory_tag _tag_) NOEXP {

    // todo:
    CORE_FATAL(0,CORE_TODO_IMPLEMENT);

}

memory_handle dynamic_allocator::allocate(u32 size, u16 alignement, memory_tag _tag_) NOEXP {

    // todo:
    CORE_FATAL(0, CORE_TODO_IMPLEMENT);

}

same_pair<memory_handle> dynamic_allocator::allocate_tow(
    memory_request const& request_1, memory_request const& request_2
) NOEXP {
    
    same_pair<memory_handle> handle;

    for (u8 i = 0; i < _blocks_count_; i++) {

        // if block is alive
        if (_blocks_[i].is_alive()) {

            // if block not busy
            if (! _blocks_[i].is_busy()) {

                // try allocate
                handle = _blocks_[i].allocate_tow(request_1 , request_2);

                // if success
                if (
                    (handle.first.response == allocator_response::success) && 
                    (handle.second.response == allocator_response::success)
                ) {
                    handle.first._block_index_  = i;
                    handle.second._block_index_ = i;

                    update_size_variables(request_1, handle.first, true);
                    update_size_variables(request_2, handle.second, true);

                    return handle;
                }
            }

        }

    }
    /*
        else mean all the block is busy at the moment or full
    */

    // try allocate new block if possible
    u8 index = add_new_block(request_1.size + request_2.size);

    // "second attempt" : try to allocate
    if (index < _capacity_) {
        handle = _blocks_[index].allocate_tow(request_1, request_2);
        handle.first._block_index_ = index;
        handle.second._block_index_ = index;
        
        update_size_variables(request_1, handle.first, true);
        update_size_variables(request_2, handle.second, true);

        return handle;
    }

    // failed to find new block or memory
    return same_pair<memory_handle>{
        memory_handle {},
        memory_handle {}
    };

}

void dynamic_allocator::deallocate(memory_handle handle) NOEXP {

    // todo: move this code to st/mt deallocate
    /*
    if (handle._block_index_ >= _capacity_) {
        #ifdef DEBUG
            CORE_ERROR_F(CORE_INDEX_OUT_OF_RANGE , handle._block_index_ , "core::dynamic_allocator");
            DEBUG_BREAK;
        #endif
        return;
    }

    memory_allocation alloc = _blocks_[handle._block_index_].get_allocation_info(handle);
    
    if (! _blocks_[handle._block_index_].deallocate(handle)) {
        CORE_WARN_F(
            "core::dynamic_allocator.deallocate(): memory block failed to deallocate {} !",
            core::pointer_to_hex_string(handle.ptr)
        );

        return;
    }

    update_size_variables(memory_request{ .size = alloc.size , .tag = (memory_tag)alloc.tag }, handle , false);
    */
}


u32 dynamic_allocator::blocks_count() NOEXP {
    return _blocks_count_;
}

u64 dynamic_allocator::memory_budget() NOEXP {
    return _memory_budget_;
}

u64 dynamic_allocator::free_memory() NOEXP {
    return _memory_budget_ - _budget_;
}

/*
    todo: implement these functions
*/
u64 dynamic_allocator::current_memory_usage() NOEXP {
    return 0;
}

u64 dynamic_allocator::current_memory_usage(memory_tag section_tag) NOEXP {
    if ((u8)section_tag < MAX_MEMORY_TAGS) {
        return _sections_[(u8)section_tag];
    }
    else return 0;
}


string const& dynamic_allocator::name() NOEXP {
    #ifdef DEBUG
        return _name_;
    #else 
        return "";
    #endif
}

subsystem_memory_tag dynamic_allocator::tag()  NOEXP {
    #ifdef DEBUG
        return _tag_;
    #else 
        return "";
    #endif
}


/*
    private helper functions
*/


/*
    note: call this function only from allocate/deallocate
*/
void core::dynamic_allocator::update_size_variables (
    memory_request const& request, memory_handle const& handle, bool increment
) NOEXP {

    // update allocator size
    if (increment) _size_.fetch_add(request.size, MEMORY_ORDER_ACQUIRE);
    else _size_.fetch_sub(request.size, MEMORY_ORDER_ACQUIRE);

#ifdef DEBUG
    // update section size
    if ((u8)request.tag < MAX_MEMORY_TAGS) {
        if(increment) _sections_[(u8)request.tag].fetch_add(request.size , MEMORY_ORDER_ACQUIRE);
        else _sections_[(u8)request.tag].fetch_sub(request.size, MEMORY_ORDER_ACQUIRE);
    }

    // update _min_ & _peak_
    core::atomic_scope_lock scope_lock(_lock_);

    u32 min  = _min_;
    u32 peak = _peak_;

    _min_  = (request.size < min)  ? request.size : min;
    _peak_ = (request.size > peak) ? request.size : peak;
#endif

}


 u8 dynamic_allocator::add_new_block(u32 target_size) NOEXP{
    
    // check memory budget
    u64 free_mem = _memory_budget_ - _budget_;
    
    if (target_size < _blocks_default_size_) target_size = _blocks_default_size_;

    if (free_mem < target_size) {
        CORE_ERROR(
            CORE_LOG_CONFIG_ALL, "dynamic_allocator {} : no memory left for {} , free={} , budget{} .",
            _name_, core::bytes_to_string(target_size) , core::bytes_to_string(free_mem) , core::bytes_to_string(_memory_budget_)
        );

        return _out_range_;
    }

    // get memory from budget
    byte* s = _seek_;
    byte* e = _seek_ + target_size;
    _seek_ = e;

    // update variables
    _budget_ -= target_size;

    // construct new block
    u8 index_ = _insert_index_.fetch_add(1, MEMORY_ORDER_ACQUIRE);
    new (_blocks_ + index_) core::memory_block(s,e, target_size);

    // add new block to free list
    for (u8 i = 0; i < _out_range_; i++) {
        if (_free_blocks_[i].index < _out_range_) {
            _free_blocks_[i] = free_block {
                index_ ,
                target_size
            };
        }
    }


    u32 block_size;

    // check size
    if (target_size >  _blocks_size_) block_size = target_size;
    if (target_size <= _blocks_size_) block_size = _blocks_size_;
    
    // check allocator memory budget
    if (_memory_budget_ < (_size_.load(MEMORY_ORDER_RELAXE) + block_size)) {
        #ifdef DEBUG
            CORE_WARN(
                CORE_LOG_CONFIG_ALL , CORE_WARNING_OUT_OF_BUDGET  CORE_WARNINIG_RUNTIME_CRASH,
                "core::dynamic_allocator" , _memory_budget_
            );

            DEBUG_BREAK;
        #endif
        return _capacity_;
    }

    if (_blocks_count_ < _capacity_) {
        if (_blocks_[_blocks_count_].alive == false) {
            u8 index = _blocks_count_;

            new (_blocks_ + index) core::memory_block(block_size, _blocks_max_allocations_ , _tag_);

            _size_ += block_size;
            _blocks_count_ += 1;

            // return new block index
            return index;
        }
        else {
            CORE_FATAL(CORE_LOG_CONFIG_ALL,
                "core::dynamic_allocator: failed to find empty spot for new block ! this could be a bug , count={} , capacity={}",
                _blocks_count_.load(MEMORY_ORDER_RELAXE) , _capacity_
            );
            return _capacity_;
        } 
    }
    else return _capacity_;
}

 memory_handle dynamic_allocator::allocate_on_st(memory_request const& request) NOEXP {
     return memory_handle();
 }

 memory_handle dynamic_allocator::allocate_on_mt(memory_request const& request) NOEXP {
     return memory_handle();
 }

 void dynamic_allocator::deallocate_on_st(memory_handle const& handle) NOEXP {

 }

 void dynamic_allocator::deallocate_on_mt(memory_handle const& handle) NOEXP {

 }

/*
void dynamic_allocator::remove_block(u8 index) NOEXP {

    if (index < _capacity_){

        if (_blocks_status_[index]) {
            _blocks_status_[index] = false;

            _size_ -= (_blocks_ + index)->size();
            
            (_blocks_ + index)->~memory_block();
            _blocks_count_ -= 1;
        }
    }

}
*/

} // namespace core end

#endif

#endif