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
    if (blocks_description.elements_count() > max_memory_blocks_allowed) {
        CORE_ERROR(
            0, "failed to create dynamic_allocator {} because {} blocks is not allowed , max=255",
            name, blocks_description.elements_count()
        );
        return;
    }

    /*
        setup memory_blocks
    */
    _blocks_ = core::c_array<memory_block>(_blocks_count_, _tag_);
    u8 i = 0;
    bool rollback = false;

    for (  ; i < _blocks_.elements_count(); i++) {
        block_description desc = blocks_description[i];

        // check block size
        if (desc.size < min_budget_allowed || desc.size > max_budget_allowed) {
            CORE_FATAL(
                0, "failed to create memory_block for dynamic_allocator {} because block with size {} is not allowed ! allowed range min={} , max={}",
                name, desc.size, min_budget_allowed, max_budget_allowed
            );

            rollback = true;
            break;
        }

        // construct block
        new (&_blocks_[i]) memory_block(desc.size , desc.max_allocations_per_block, desc.tag);

        // update variables
        _memory_budget_ += desc.size;

        // check memory budget
        if (_memory_budget_ > max_budget_allowed) {
            CORE_FATAL(
                0, "dynamic_allocator {} is out of budget {} ! allowed range min={} , max={}",
                name, _memory_budget_, min_budget_allowed, max_budget_allowed
            );

            rollback = true;
            break;
        }

    }

    if (rollback) {
        CORE_WARN(
            0, "dynamic_allocator {} giving back 'memory_blocks' because it failed to create some of them properly !", name
        );

        for (; i >= 0; i--) {
            _blocks_[i].~memory_block();
        }
    }

}

/*
	destructor
*/

dynamic_allocator::~dynamic_allocator() NOEXP {

    _blocks_count_  = 0;
    _memory_budget_ = 0;

    CORE_DEBUG(
        0, "dynamic_allocator {} with budget {} used for {} destructed !" ,
        _name_ , _memory_budget_ , core::to_string(_tag_)
    );
}

/*
    public functions
*/

memory_handle dynamic_allocator::allocate(memory_request request) NOEXP {
    
    /*
        todo: keep the variables updateed with each allocation/deallocation
    */

    if (request.block_index < _blocks_count_) {    
        return _blocks_[request.block_index].allocate(request);
    }

    return memory_handle{};
}

memory_handle dynamic_allocator::allocate(u32 size, memory_tag tag, u8 block_index, bool wait_for_block) NOEXP {

    return this->allocate(
        memory_request{ 
            .size = size,
            .alignement = 0,
            .tag  = tag,
            .block_index = block_index,
            .wait_for_memory_block = wait_for_block
        }
    );

}

memory_handle dynamic_allocator::allocate(
    u32 size, u16 alignement, memory_tag tag, u8 block_index, bool wait_for_block
) NOEXP {

    return this->allocate(
        memory_request{
            .size = size,
            .alignement = alignement,
            .tag = tag,
            .block_index = block_index,
            .wait_for_memory_block = wait_for_block
        }
    );

}

same_pair<memory_handle> dynamic_allocator::allocate_tow(
    memory_request const& request_1, memory_request const& request_2
) NOEXP {
    
    if (request_1.block_index < _blocks_count_ && request_2.block_index < _blocks_count_) {
        return same_pair<memory_handle> {
            .first  = _blocks_[request_1.block_index].allocate(request_1),
            .second = _blocks_[request_2.block_index].allocate(request_2)
        };
    }

    return same_pair<memory_handle>();
}

void dynamic_allocator::deallocate(memory_handle handle) NOEXP {

#ifdef DEBUG
    if (handle._block_index_ < _blocks_count_) {
        _blocks_[handle._block_index_].deallocate(handle);
        return;
    }

    CORE_ERROR(CORE_LOG_CONFIG_ALL, "attempt to deallocate invalid handle using dynamic_allocator {}" , _name_);
    CORE_WARN(0, CORE_WARNINIG_RUNTIME_CRASH);
#else

    _blocks_[handle._block_index_].deallocate(handle);

#endif

}


u32 dynamic_allocator::blocks_count() NOEXP {
    return _blocks_count_;
}

u64 dynamic_allocator::memory_budget() NOEXP {
    return _memory_budget_;
}

u64 dynamic_allocator::free_memory() NOEXP {
    u64 free_mem = 0;

    // note: this is O(N) right now , upgrade to better solution later
    for (u8 i = 0; i < _blocks_count_; i++) {
        free_mem += _blocks_[i].free_memory();
    }

    return free_mem;
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


} // namespace core end

#endif

#endif