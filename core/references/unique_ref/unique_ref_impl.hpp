#pragma once 

#ifndef CORE_UNIQUE_REF_IMPL_HPP
#define CORE_UNIQUE_REF_IMPL_HPP

#include "core/logger/logger.hpp"
#include "unique_ref.hpp"

#ifdef DEBUG
	static inline auto unique_ref_logger = CORE_GET_LOGGER(REFS_LOGGER);
#else 
	static inline auto unique_ref_logger = nullptr;
#endif

#define UNIQUE_REF_TEMPLATE template<typename type, core::memory::management plan>

/*
	constructor's
*/

UNIQUE_REF_TEMPLATE
unique_ref<type, plan>::unique_ref(type* pointer) NOEXP REQUIRE_MEMORY_MANAGEMENT_BY_IT_SELF {
#ifdef DEBUG
	if (pointer == nullptr) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "nullptr passed to unique_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE_HPP(unique_ref_logger);
	}
#endif

	this->memory = pointer;
}

UNIQUE_REF_TEMPLATE 
unique_ref<type, plan>::unique_ref(type* pointer, core::memory_allocator const& allocator) NOEXP 
	REQUIRE_MEMORY_MANAGEMENT_BY_OTHERS {

#ifdef DEBUG
	if (pointer == nullptr) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "nullptr passed to unique_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE_HPP(unique_ref_logger);
	}
#endif

	this->memory    = pointer;
	this->allocator = allocator;
}

UNIQUE_REF_TEMPLATE unique_ref<type,plan>::unique_ref(unique_ref<type,plan>&& other) NOEXP {
#ifdef DEBUG
	if (other.memory == nullptr) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "nullptr passed to unique_ref<{}> during construction time !", type_name);
		CORE_STACK_TRACE_HPP(unique_ref_logger);
	}
#endif

	if constexpr (plan == BY_OTHERS) {
		this->allocator = other.allocator;
	}

	this->memory = other.memory;
	other.memory = nullptr;
}


/*
			destructor
*/

UNIQUE_REF_TEMPLATE
unique_ref<type,plan>::~unique_ref() NOEXP {

	if (this->memory) {

		if constexpr (plan == core::memory::management::by_self) {
			// note: self mananged types should deallocate their on memory during destruction time
			this->memory->~type();
		}
		else {
			// destruct + deallocate
			this->memory->~type();
			this->allocator->deallocate(this->memory);
			this->allocator = nullptr;
		}

		this->memory = nullptr;

		return;
	}

#ifdef DEBUG
	CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "unique_ref<{}> pointer was nullptr in destruction time !", type_name);
#endif
}


/*
			operator's
*/

UNIQUE_REF_TEMPLATE 
unique_ref<type,plan>& unique_ref<type,plan>::operator= (type* pointer) NOEXP REQUIRE_MEMORY_MANAGEMENT_BY_IT_SELF {

	if (pointer == nullptr) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "nullptr passed to unique_ref<{}> using 'operator =' !", type_name);
		return *this;
	}

	if (this->memory == pointer) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "same pointer passed to unique_ref<{}> using 'operator =' !", type_name);
		return *this;
	}

	if (this->memory) {
		this->memory->~type();
	}
	
	this->memory = pointer;

	return *this;
}

UNIQUE_REF_TEMPLATE 
unique_ref<type,plan>& unique_ref<type,plan>::operator= (unique_ref<type,plan>&& other) NOEXP {

	if (other.memory == nullptr) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "nullptr passed to unique_ref<{}> using 'operator =' !", type_name);
		return *this;
	}

	if (this->memory == other.memory || this == &other) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL , "self-assignement attempt to unique_ref<{}> using 'operator =' !" , type_name);
		return *this;
	}

	if constexpr (plan == BY_OTHERS) {

		if (this->memory) {
			this->memory->~type();
			this->allocator->deallocate(this->memory);
		}

		this->allocator = other.allocator;
		this->memory    = other.memory;

		other.memory    = nullptr;
		other.allocator = nullptr;
	}
	else {

		if (this->memory) this->memory->~type();

		this->memory = other.memory;
		other.memory = nullptr;
	}
	
	return *this;
}


UNIQUE_REF_TEMPLATE type* unique_ref<type,plan>::operator->() NOEXP {
#ifdef DEBUG
	// note: this will trigger 'debugger' in 'debug-build'
	if (this->memory == nullptr) {
		CORE_FATAL_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL , "attempt to access nullptr unique_ref<{}> !", type_name);
	}
#endif

	return this->memory;
}

UNIQUE_REF_TEMPLATE type& unique_ref<type,plan>::operator*() NOEXP {

#ifdef DEBUG  // note: this will trigger 'debugger' in 'debug-build'
	if (this->memory == nullptr) {
		CORE_FATAL_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "attempt to dereference nullptr unique_ref<{}> !" , type_name);
	}
#endif

	return *this->memory;
}

UNIQUE_REF_TEMPLATE const type& unique_ref<type,plan>::operator*() const NOEXP {

#ifdef DEBUG  // this will trigger debugger in debug build
	if (this->memory == nullptr) {
		CORE_FATAL_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "attempt to dereference nullptr unique_ref<{}> !", type_name);
	}
#endif

	return *this->memory;
}


UNIQUE_REF_TEMPLATE unique_ref<type,plan>::operator bool() const NOEXP {
	return (this->memory != nullptr);
}

/*
				function's
*/

UNIQUE_REF_TEMPLATE
type* unique_ref<type,plan>::pass_ownership() NOEXP REQUIRE_MEMORY_MANAGEMENT_BY_IT_SELF{

	type* ptr = this->memory;
	this->memory = nullptr;

	return ptr;
}


UNIQUE_REF_TEMPLATE 
bool unique_ref<type,plan>::move_ownership(unique_ref<type,plan>& ref, unique_ref<type,plan>& new_owner) {

	if (ref.memory == nullptr) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "moving-ownership 'nullptr' unique_ref is not allowed!");
		return false;
	}

	if (ref.memory == new_owner.memory || &ref == &new_owner) {
		CORE_WARN_HPP(unique_ref_logger, CORE_LOG_CONFIG_ALL, "moving-ownership from unique_ref to it's self is probablly caused by a bug !");
		return false;
	}

	// clean for new reference
	if (new_owner.memory) {
		new_owner.memory->~type();
	}

	// pass ownership
	if constexpr (plan == BY_OTHERS) {
		new_owner.allocator = ref.allocator;
	}

	new_owner.memory = ref.memory;
	ref.memory = nullptr;

	return true;
}


namespace core {
	
namespace make {

	template<typename type, core::memory::management plan , typename... parameters> 
	requires refcounted_type<type, plan>

	unique_ref<type, plan> unique_ref_(
		core::memory_allocator const& allocator, parameters&&... constructor_parameters
	) NOEXP {

		// allocate memory
		type* pointer = (type*)allocator.allocate(sizeof(type));
		// construct memory
		new (pointer) type( std::forward<parameters>(constructor_parameters)... );

		// pass allocator to the object if it self mananging memory
		if constexpr(plan == BY_IT_SELF) {
			pointer->allocator = (core::memory_allocator*)&allocator;
			return unique_ref<type, plan>(pointer);
		}
		else return unique_ref<type,plan>(pointer, allocator);
	}


} // namespace make end

} // namespace core end


#endif