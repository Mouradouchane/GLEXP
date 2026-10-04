#pragma once 

#ifndef CORE_CARRAY_HPP
#define CORE_CARRAY_HPP

#include "core/types.hpp"
#include "core/logger/logger.hpp"
#include "core/memory/memory.hpp"

#ifdef DEBUG
	static inline auto _carr_hpp_lgr_ = CORE_GET_LOGGER(DATA_STRUCTER_LOGGER);
#else 
	static inline auto _carr_hpp_lgr_ = nullptr;
#endif

namespace core {

/*
	simple array for global usage

	NOTE: - c_array is not safe , there's no check no type safety .
		    - designed for full control and access .
*/
template<typename type> class c_array {
	private:
		g_memory_handle handle;
		u32   count  = NULL;
		u64   size   = NULL;
		type* start  = nullptr;
		type* end    = nullptr;
		bool  alive  = false;
		subsystem_memory_tag tag = subsystem_memory_tag::unkown;

	public:
		c_array( ) = default;
		c_array(const u32 count_, subsystem_memory_tag tag_) NOEXP {
			handle = core::memory::allocate(
				g_memory_request{ .size = sizeof(type) * count_ , .tag = tag_ }
			);

			if (handle.response != allocator_response::success) return;

			this->count = count_;
			this->size  = sizeof(type) * count_;
			start = (type*)handle.pointer;
			end   = start + count;
			tag   = tag_;

			alive = true;
		}

		~c_array() {
			if (alive) {
				core::memory::deallocate(this->handle);
				start = nullptr;
				end   = nullptr;
				alive = false;
			}
		}

		u64 size_() NOEXP;
		u32 elements_count() NOEXP;
		
		type* begin() NOEXP;
		type* end() NOEXP;

		// operator's
		type& operator[](u32 index) NOEXP {
		#ifdef DEBUG
			if (index < count) return *(this->start + index);
			else {
				CORE_ERROR_HPP(_carr_hpp_lgr_, 0, "c_array index {} out of range {} !", index , count);
				return *(this->end + 1);
			}
		#else
			return *(this->start + index);
		#endif

		}
		
	}; // calss c_array end

} // namespace core end

#endif