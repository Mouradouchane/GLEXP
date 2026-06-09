#pragma once 

#ifndef CORE_MEMORY_REGISTRE_CPP
#define CORE_MEMORY_REGISTRE_CPP

#include "core/logger/logger.hpp"

#include "registery.hpp"

#ifdef DEBUG
	static auto _core_registry_alloc_logger_ = CORE_GET_LOGGER(MEMORY_ALLOCATOR_LOGGER);
#else 
	static auto _core_registry_alloc_logger_ = nullptr;
#endif

#define _LOGGER_  _core_registry_alloc_logger_

#define ZERO_SIZE_REGISTRY "zero size registry is not allowed , auto adjust to default size ."
#define REGISTRY_FAILED_TO_INSERT "registry failed to insert ({},{}) !"
#define REGISTRY_IS_FULL        "registery is full , failed to insert ."
#define REGISTRY_PTR_NOT_FOUND  "registry: pointer {} not found in registry ."


namespace core {

	namespace memory {

		/*
				constructor
		*/ 

		registry::registry() NOEXP {

			this->capacity = registry::default_register_capacity;
			this->size = sizeof(core::memory_allocation) * this->capacity;

			this->list = (core::memory_allocation*) core::memory::allocate(
				core::g_memory_request {
					this->size, 
					(u8)core::memory_tag::registry
				}
			);

			this->allocations_count = 0;
			this->allocations_size  = 0;

			if (this->list) {
				CORE_DEBUG(0,"new registry created with capacity={} .", this->capacity);
			}

		}

		registry::registry(u32 register_capacity) NOEXP {

			this->capacity = (register_capacity) ? register_capacity : registry::default_register_capacity;
			this->size = sizeof(core::memory_allocation) * this->capacity;

			this->list = (core::memory_allocation*)core::memory::allocate(
				core::g_memory_request {
					this->size,
					(u8)core::memory_tag::registry
				}
			);

			this->allocations_count = 0;
			this->allocations_size  = 0;
			
			if (this->list) {
				CORE_DEBUG(0, "new registry created with capacity={} .", this->capacity);
			}
		}

		/*
			destructor
		*/

		registry::~registry() NOEXP {

			// [WARNNING] : any allocations left registred in register will be lost !
			core::memory::deallocate(this->list);

			this->size = 0;

			CORE_DEBUG(0,"registry with capacity={} desturcted !", this->capacity);
			this->capacity = 0;

		}


		/*
			registry public functions
		*/

		u32 registry::insert(void* ptr, u32 size, u8 tag) NOEXP {

			if (this->allocations_count < this->capacity) {

				// hash then search for empty place to insert new allocation
				u32 index = this->hash_pointer(ptr);
				u32 i = this->search(index, nullptr);

				if (i < this->capacity) {
				#ifdef DEBUG
					this->list[i] = core::memory_allocation{ ptr , size , tag };
				#else 
					this->list[i] = memory_allocation{ ptr , size };
				#endif

					this->allocations_count += 1;
					this->allocations_size  += size;

					return i;
				}
				else { // failed to find empty place in registry 
					CORE_FATAL(CORE_LOG_CONFIG_ALL, REGISTRY_FAILED_TO_INSERT, core::pointer_to_hex_string(ptr), size);
					return this->capacity;
				}

			}
			else {
				CORE_WARN(CORE_LOG_CONFIG_ALL, REGISTRY_IS_FULL);
				return this->capacity;
			}

		}


		bool registry::remove(void* ptr) NOEXP {

			// hash then search for ptr
			u32 index = this->hash_pointer(ptr);
			u32 i = this->search(index, ptr);

			if (i < this->capacity) {
				this->allocations_count -= 1;
				this->allocations_size  -= this->list[i].size;

				this->list[i].clear();

				return true;
			}
			else { // ptr not found
				CORE_WARN(CORE_LOG_CONFIG_ALL, REGISTRY_PTR_NOT_FOUND, core::pointer_to_hex_string(ptr));
				return false;
			}

		}


		core::memory_allocation registry::cut(void* ptr) NOEXP {

			// hash then search for ptr
			u32 index = this->hash_pointer(ptr);
			u32 i = this->search(index, ptr);

			if (i < this->capacity) {
				core::memory_allocation alloc = this->list[i];

				this->allocations_count -= 1;
				this->allocations_size  -= this->list[i].size;

				this->list[i].clear();

				return alloc;
			}
			else { // ptr not found
				CORE_WARN(CORE_LOG_CONFIG_ALL, REGISTRY_PTR_NOT_FOUND, core::pointer_to_hex_string(ptr));
				return core::memory_allocation{ 0 };
			}

		}

		u32 registry::exist(void* ptr) NOEXP {
			return this->search(this->hash_pointer(ptr), ptr);
		}

		INLINE u32 registry::get_size() NOEXP {
			return this->size;
		}

		INLINE u32 registry::get_capacity() NOEXP {
			return this->capacity;
		}

		INLINE u32 registry::get_allocations_count() NOEXP {
			return this->allocations_count;
		}

		INLINE u32 registry::get_allocations_size()  NOEXP {
			return this->allocations_size;
		}


		/*
			registry helper functions
		*/

		// todo: get some good hashing function later -> https://github.com/cyan4973/xxhash
		u32 registry::hash_pointer(void* ptr) NOEXP {

			return u32((u64)ptr % this->capacity);
		}

		u32 registry::search(u32 start_index = 0, void* ptr = nullptr) NOEXP {

			u32 i = start_index;

			// search from start_index to end
			for (; i < this->capacity; i++) {
				if (this->list[i].ptr == ptr) return i;
			}

			// search from 0 to start_index
			i = 0;
			for (; i < start_index; i++) {
				if (this->list[i].ptr == ptr) return i;
			}

			// not found
			return this->capacity;
		}


	} // namespace memory end


	INLINE void core::memory_allocation::clear() NOEXP {

	#ifdef DEBUG
		this->ptr  = nullptr;
		this->size = 0;
		this->tag  = (u8)core::memory_tag::unkown;
	#else 
		this->ptr  = nullptr;
		this->size = 0;
	#endif

	}

} // namesapce core end

#endif