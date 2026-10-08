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

dynamic_allocator::dynamic_allocator(
    string const& name, const u64 blocks_size, const u8 blocks_count, const u16 max_allocations_per_block , const subsystem_memory_tag tag
) NOEXP {

    // set variables
    _name_ = name;
    _tag_  = tag;
    _blocks_count_  = blocks_count ? blocks_count : 1;
    _block_size_    = blocks_size;
    _memory_budget_ = _block_size_ * _blocks_count_;

    // check memory budget
    if (_memory_budget_ < min_budget_allowed || _memory_budget_ > max_budget_allowed) {
        CORE_ERROR(
            0,"failed to create dynamic_allocator {} because memory budget {} is not allowed ! allowed range min={} , max={}",
            name, _memory_budget_, min_budget_allowed , max_budget_allowed
        );
        return;
    }

    // create and init memory_blocks
    _blocks_ = core::c_array<memory_block>(_blocks_count_ , _tag_);

    for (u8 i = 0; i < _blocks_.elements_count(); i++ ) {
        new (&_blocks_[i]) memory_block(_block_size_, max_allocations_per_block, _tag_);
    }

}

dynamic_allocator::dynamic_allocator(
    string const& name, c_array<block_description> blocks_description, const subsystem_memory_tag tag
) NOEXP {

    // set variables
    _name_ = name;
    _tag_  = tag;
    _blocks_count_  = blocks_description.elements_count() ? blocks_description.elements_count() : 1;

    // check blocks count
    if (blocks_description.elements_count() > 255) {
        CORE_ERROR(
            0, "failed to create dynamic_allocator {} because {} blocks is not allowed , max=255",
            name, blocks_description.elements_count()
        );
        return;
    }

    // create and init memory_blocks
    _blocks_ = core::c_array<memory_block>(_blocks_count_, _tag_);

    for (u8 i = 0; i < _blocks_.elements_count(); i++) {
        block_description desc = blocks_description[i];

        // check block size
        if (desc.size < min_budget_allowed || desc.size > max_budget_allowed) {
            CORE_ERROR(
                0, "failed to create memory_block for dynamic_allocator {} because block size {} is not allowed ! allowed range min={} , max={}",
                name, _memory_budget_, min_budget_allowed, max_budget_allowed
            );
        }

        new (&_blocks_[i]) memory_block(_block_size_, max_allocations_per_block, _tag_);
    }

    // check memory budget
    if (_memory_budget_ < min_budget_allowed || _memory_budget_ > max_budget_allowed) {
        CORE_ERROR(
            0, "failed to create dynamic_allocator {} because memory budget {} is not allowed ! allowed range min={} , max={}",
            name, _memory_budget_, min_budget_allowed, max_budget_allowed
        );



        return;
    }
}

/*
	destructor
*/

dynamic_allocator::~dynamic_allocator() NOEXP {

    core::memory::deallocate(_handle_);
    
    _blocks_  = nullptr;
    _blocks_count_  = 0;
    _memory_budget_ = 0;

    CORE_DEBUG(0, "core::dynamic_allocator {} destructed !" , _name_ );
}

/*
    public functions
*/

memory_handle dynamic_allocator::allocate(memory_request request) NOEXP {
    
    memory_handle handle;
    bool expected = false; 

    // loop over all the free blocks
    for (u8 i = 0; i < _blocks_count_; i++) {
    
        // if free_block parameters is good for "request"
        if (_free_blocks_[i].index < _out_range_ && _free_blocks_[i].free_memory >= request.size) {

            // try to take the block
            if (_free_blocks_[i].lock.compare_exchange_strong(expected, true, MEMORY_ORDER_ACQUIRE)) {
                
                // try allocate memory
                handle = _blocks_[ _free_blocks_[i].index ].allocate(request);
                
                // release block
                _free_blocks_[i].lock = false;

                if (handle.response == allocator_response::success) {
                    break;
                }
            }
            else continue;

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

    /*
        else mean all the block is busy at the moment or full
    */
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

    // update variables
    _seek_    = e;
    _budget_ -= target_size;
    u8 index_ = _blocks_count_.fetch_add(1, MEMORY_ORDER_ACQUIRE);

    // create new block
    new (_blocks_ + index_) core::memory_block(s,e, target_size , _tag_);

    // add new block to free list
    for (u8 i = 0; i < _out_range_; i++) {

        if (_free_blocks_[i].index >= _out_range_) {
            new (_free_blocks_ + i) free_block(index_ , target_size);
            return i;
        }
    }

    CORE_ERROR(CORE_LOG_CONFIG_ALL, "dynamic_allocator {}: failed to find empty spot in free list for new memory_block !", _name_);
    return _out_range_;
}


} // namespace core end

#endif

#endif