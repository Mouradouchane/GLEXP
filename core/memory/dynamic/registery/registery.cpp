#pragma once 

#ifndef CORE_MEMORY_REGISTRE_CPP
#define CORE_MEMORY_REGISTRE_CPP

#include "registery.hpp"
#include "core/memory/global_allocator/global_allocator.hpp"
#include "core/logger/logger.hpp"

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

			this->capacity = next_power_of_two(registry::default_register_size | 1);

			this->size = sizeof(core::memory::allocation) * this->capacity;
			this->list = (core::memory::allocation*)core::memory::allocate(this->size, core::memory::tag::memory);

			CORE_INFO("new registry created with capacity={} .", this->capacity);
		}

		registry::registry(u32 register_capacity) NOEXP {

			this->capacity = next_power_of_two(register_capacity | 1);

			this->size = sizeof(core::memory::allocation) * this->capacity;
			this->list = (core::memory::allocation*)core::memory::allocate(this->size, core::memory::tag::memory);

			CORE_INFO("new registry created with capacity={} .", this->capacity);
		}

		/*
			destructor
		*/

		registry::~registry() NOEXP {

			core::memory::deallocate(this->list);

			this->size = 0;
			this->count = 0;

			CORE_INFO("registry with capacity={} desturcted !", this->capacity);
			this->capacity = 0;

		}


		/*
			registry public functions
		*/

		u32 registry::insert(void* ptr, u32 size, core::memory::tag tag_) NOEXP {

			if (this->count < (this->capacity - 1)) {

				// hash then search for empty place
				u32 index = this->hash_pointer(ptr);
				u32 i = this->search(index, nullptr);

				if (i < this->capacity) {
				#ifdef DEBUG
					this->list[i] = allocation{ ptr , size , tag_ };
				#else 
					this->list[i] = allocation{ ptr , size };
				#endif

					this->count += 1;
					this->allocations_size += size;

					return i;
				}
				else { // no place found
					CORE_ERROR(CORE_LOG_CONFIG_ALL, REGISTRY_FAILED_TO_INSERT, core::pointer_to_hex_string(ptr), size);
					return this->count;
				}

			}
			else {
				CORE_WARN(CORE_LOG_CONFIG_ALL, REGISTRY_IS_FULL);
				return this->count;
			}

		}


		bool registry::remove(void* ptr) NOEXP {

			// hash then search for ptr
			u32 index = this->hash_pointer(ptr);
			u32 i = this->search(index, ptr);

			if (i < this->capacity) {
				this->count -= 1;
				this->allocations_size -= this->list[i].size;

				this->list[i].clear();

				return true;
			}
			else { // no place found
				CORE_WARN(CORE_LOG_CONFIG_ALL, REGISTRY_PTR_NOT_FOUND, core::pointer_to_hex_string(ptr));
				return false;
			}

		}


		core::memory::allocation registry::cut(void* ptr) NOEXP {

			// hash then search for ptr
			u32 index = this->hash_pointer(ptr);
			u32 i = this->search(index, ptr);

			if (i < this->capacity) {
				core::memory::allocation alloc = this->list[i];

				this->count -= 1;
				this->allocations_size -= this->list[i].size;

				this->list[i].clear();

				return alloc;
			}
			else {
				CORE_WARN(CORE_LOG_CONFIG_ALL, REGISTRY_PTR_NOT_FOUND, core::pointer_to_hex_string(ptr));
				return core::memory::allocation{ 0 };
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
			return this->count;
		}

		INLINE u32 registry::get_allocations_size()  NOEXP {
			return this->allocations_size;
		}


		/*
			registry helper functions
		*/

		u32 registry::hash_pointer(void* ptr) NOEXP {

			u64 iptr = (u64)ptr;

			// 1. Shift RIGHT to discard the always-zero lower alignment bits
			iptr = iptr >> 3;

			// 2. Mix with a prime factor to destroy predictable address patterns 
			//    and prevent massive collision clusters.
			u64 mix = iptr * 11400714819323198485ULL;

			// 3. Cleanly wrap the index within your exact registry length range
			return u32(mix % this->capacity);
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


		INLINE void allocation::clear() NOEXP {

#ifdef DEBUG
			this->ptr = nullptr;
			this->size = 0;
			this->tag_ = tag::unkown;
#else 
			this->ptr = nullptr;
			this->size = 0;
#endif

		}

		static INLINE u32 next_power_of_two(u32 value) NOEXP {
			value--;
			value |= value >> 1;
			value |= value >> 2;
			value |= value >> 4;
			value |= value >> 8;
			value |= value >> 16;
			value++;

			return value;
		}


	} // namespace memory end


} // namesapce core end

#endif