#pragma once 

#ifndef CORE_SHARED_REF_IMPL
#define CORE_SHARED_REF_IMPL

#include "shared_ref.hpp"

#ifdef DEBUG
	static auto _shared_ref_logger = CORE_GET_LOGGER(REFS_LOGGER);
#else
	static auto _shared_ref_logger = nullptr;
#endif

#define SHARED_REF_TEMPLATE template<typename type>
#define DERIVED_SHARED_REF_TEMPLATE template<std::derived_from<type> derived_type> 


/*

	shared_ref constructor's

*/

SHARED_REF_TEMPLATE
shared_ref<type>::shared_ref(core::memory_allocator& allocator_, counter_block* ctr_ptr, type* memory_ptr) {

	if (ctr_ptr && memory_ptr) {
		this->ctr    = ctr_ptr;
		this->ctr->allocator = (core::memory_allocator*)&allocator_;
		this->memory = memory_ptr;

		ADD_SHARED_REF_ATOMIC(this);
	}
	else {
		if (!ctr_ptr) {
			CORE_FATAL_HPP(_shared_ref_logger, CORE_LOG_CONFIG_ALL, REF_INVALID, "counter-block is nullptr");
		}
		else {
			CORE_FATAL_HPP(_shared_ref_logger, CORE_LOG_CONFIG_ALL, REF_INVALID, "memory is nullptr");
		}
	}

}

SHARED_REF_TEMPLATE
template<typename... parameters>
shared_ref<type>::shared_ref(core::memory_allocator& _allocator_, parameters&&... constructor_parameters) NOEXP {

	two_pointers ptrs = _allocator_.allocate_tow(sizeof(type) , sizeof(counter_block));

	this->memory = (type*)ptrs.ptr1;
	new (this->memory) type( std::forward<parameters>(constructor_parameters)... );

	this->ctr = (counter_block*)ptrs.ptr2;
	this->ctr->allocator = (core::memory_allocator*)&_allocator_;

	ADD_SHARED_REF_ATOMIC(this);
}

SHARED_REF_TEMPLATE
shared_ref<type>::shared_ref(shared_ref<type> const& reference) NOEXP { // copy

	if (reference.memory && reference.ctr) {
		this->ctr    = reference.ctr;
		this->memory = reference.memory;

		ADD_SHARED_REF_ATOMIC(this);
	}
	else {
		CORE_FATAL_HPP(_shared_ref_logger, CORE_LOG_CONFIG_ALL , REF_INVALID , "shared_ref memory or counter-block is nullptr");
	}

}

SHARED_REF_TEMPLATE
shared_ref<type>::shared_ref(shared_ref<type>&& reference) NOEXP { // move

	if (reference.memory && reference.ctr) {
		this->ctr    = reference.ctr;
		this->memory = reference.memory;

		reference.ctr    = nullptr;
		reference.memory = nullptr;
	}
	else {
		CORE_FATAL_HPP(_shared_ref_logger, CORE_LOG_CONFIG_ALL, REF_INVALID, "shared_ref memory or counter-block is nullptr");
	}

}

/*

	shared_ref upcasting constructor's

*/

// note: use this for upcasting "base <-- derived"
SHARED_REF_TEMPLATE
DERIVED_SHARED_REF_TEMPLATE
shared_ref<type>::shared_ref(shared_ref<derived_type> const& reference) NOEXP { // copy

	if (reference.memory && reference.ctr) {
		this->memory = reference.memory;
		this->ctr    = reference.ctr;

		ADD_SHARED_REF_ATOMIC(this);
	}
	else {
		CORE_FATAL_HPP(_shared_ref_logger, CORE_LOG_CONFIG_ALL, REF_INVALID, "shared_ref<derived> memory or counter-block is nullptr");
	}

}

SHARED_REF_TEMPLATE
DERIVED_SHARED_REF_TEMPLATE 
shared_ref<type>::shared_ref(shared_ref<derived_type>&& reference) NOEXP { // move

	if (reference.memory && reference.ctr) {
		this->memory = reference.memory;
		this->ctr    = reference.ctr;

		reference.memory = nullptr;
		reference.ctr    = nullptr;
	}
	else {
		CORE_FATAL_HPP(_shared_ref_logger, CORE_LOG_CONFIG_ALL, REF_INVALID, "shared_ref<derived> memory or counter-block is nullptr");
	}

}

/*

	class shared_ref destructor

*/

SHARED_REF_TEMPLATE
shared_ref<type>::~shared_ref() {

	if (this->memory && this->ctr) {

		// when no refernce is left
		if (SUB_SHARED_REF_ATOMIC(this) == 1 && (this->ctr->__weak__ == 0)) {

			this->memory->~type();

			this->ctr->allocator->deallocate(this->memory);
			this->ctr->allocator->deallocate(this->ctr);

			this->memory = nullptr;
			this->ctr    = nullptr;
		}

	}
#ifdef DEBUG
	else {
		CORE_WARN_HPP(_shared_ref_logger, CORE_LOG_CONFIG_ALL ,REF_INVALID , "memory-block or counter-block is nullptr when destructor is called !");
	}
#endif

}


/*

	shared_ref public functions

*/


// note: use this to do downcasting "base --> derived"
// note: don't use dynamic_cast_to a lot to avoid preformance costs
SHARED_REF_TEMPLATE 
template<typename family_type> 
shared_ref<family_type> shared_ref<type>::dynamic_cast_to() {

	COMPILE_TIME_ASSERT(
		(!std::is_base_of_v<type, family_type> && !std::is_base_of_v<family_type, type>) ,
		"shared_ref<type> is not related !"
	);
	
	return shared_ref<family_type>( this->ctr , D_CAST(this->memory, family_type*) );
}


// inline heleper function used to remove and clean current reference 
// to make sure place is ready for new one .
SHARED_REF_TEMPLATE
INLINE void shared_ref<type>::deal_with_current_reference() NOEXP {

	if (this->memory && this->ctr) {

		// handel memory-block
		if (SUB_SHARED_REF_ATOMIC(this) == 1) {
			this->memory->~type(); // destruct 
			this->ctr->allocator->deallocate(this->memory); // deallocate
		}

		// handel counter-block
		if (this->ctr->__strong__ == 0 && this->ctr->__weak__ == 0) {
			this->ctr->allocator->deallocate(this->ctr); // destruct
		}

	}
	
	this->ctr    = nullptr;
	this->memory = nullptr;

}

/*

	class shared_ref operator's

*/

// assign operator
SHARED_REF_TEMPLATE
shared_ref<type>& shared_ref<type>::operator= (shared_ref<type> const& reference) NOEXP { // copy
	
	if (this->memory == reference.memory) {
		CORE_WARN_HPP(_shared_ref_logger, CORE_LOG_CONFIG_ALL, REF_SELF_ASSIGN);
		return *this;
	}
	
	if(reference.memory && reference.ctr) {
		shared_ref<type>::deal_with_current_refernce();

		this->ctr = reference.ctr;
		ADD_SHARED_REF_ATOMIC(this);

		this->memory = reference.memory;
	}
	else {
		CORE_FATAL_HPP(_shared_ref_logger, CORE_LOG_CONFIG_ALL, REF_INVALID, "because memory-block or counter-block is nullptr !");
	}

	return *this;
}

SHARED_REF_TEMPLATE
shared_ref<type>& shared_ref<type>::operator= (shared_ref<type>&& reference) NOEXP { // move

	if (this->memory == reference.memory) {
		CORE_WARN_HPP(_shared_ref_logger, CORE_LOG_CONFIG_ALL, REF_SELF_ASSIGN);
		return *this;
	}

	if (reference.memory && reference.ctr) {
		shared_ref<type>::deal_with_current_refernce();

		this->ctr = reference.ctr;
		ADD_SHARED_REF_ATOMIC(this);

		this->memory = reference.memory;

		reference.memory = nullptr;
		reference.ctr = nullptr;
	}
	else {
		CORE_FATAL_HPP(_shared_ref_logger, CORE_LOG_CONFIG_ALL, REF_INVALID, "because memory or counter-block is nullptr !");
	}

	return *this;
}

SHARED_REF_TEMPLATE
DERIVED_SHARED_REF_TEMPLATE
shared_ref<type>& shared_ref<type>::operator=(shared_ref<derived_type> const& reference) NOEXP {

	if (this->memory == reference.memory) {
		CORE_WARN_HPP(_shared_ref_logger, CORE_LOG_CONFIG_ALL, REF_SELF_ASSIGN);
		return *this;
	}

	if (reference.memory && reference.ctr) {
		shared_ref<type>::deal_with_current_refernce();

		this->ctr    = reference.ctr;
		ADD_SHARED_REF_ATOMIC(this);

		this->memory = D_CAST(reference.memory, type*);
	}
	else {
		CORE_FATAL_HPP(_shared_ref_logger, CORE_LOG_CONFIG_ALL, REF_INVALID, "because memory or counter-block is nullptr !");
	}

	return *this;
}

SHARED_REF_TEMPLATE
DERIVED_SHARED_REF_TEMPLATE
shared_ref<type>& shared_ref<type>::operator=(shared_ref<derived_type>&& reference) NOEXP {

	if (this->memory == reference.memory) {
		CORE_WARN_HPP(_shared_ref_logger, CORE_LOG_CONFIG_ALL, REF_SELF_ASSIGN);
		return *this;
	}

	if (reference.memory && reference.ctr) {
		shared_ref<type>::deal_with_current_refernce();

		this->ctr    = reference.ctr;
		ADD_SHARED_REF_ATOMIC(this);

		this->memory = D_CAST(reference.memory , type*);

		reference.memory = nullptr;
		reference.ctr = nullptr;
	}
	else {
		CORE_FATAL_HPP(_shared_ref_logger, CORE_LOG_CONFIG_ALL, REF_INVALID, "because memory or counter-block is nullptr !");
	}

	return *this;
}

SHARED_REF_TEMPLATE
type* shared_ref<type>::operator-> () NOEXP {

#ifdef DEBUG
	if (this->memory) return this->memory;
	else {
		CORE_ERROR_HPP(_shared_ref_logger, CORE_LOG_CONFIG_ALL, REF_INVALID_MEMORY);
		return &shared_ref<type>::____dummy____;
	}
#else 
	return this->memory;
#endif

}

SHARED_REF_TEMPLATE
const type* shared_ref<type>::operator-> () const NOEXP {

#ifdef DEBUG
	if (this->memory) return C_CAST(this->memory, const type*);
	else {
		CORE_ERROR_HPP(_shared_ref_logger, CORE_LOG_CONFIG_ALL, REF_INVALID_MEMORY);
		return &shared_ref<type>::__dummy__;
	}
#else
	return C_CAST(this->memory, const type*);
#endif

}

SHARED_REF_TEMPLATE
type& shared_ref<type>::operator* () NOEXP {

#ifdef DEBUG
	if (this->memory) return *this->memory;
	else {
		CORE_ERROR_HPP(_shared_ref_logger, CORE_LOG_CONFIG_ALL, REF_INVALID_MEMORY);
		return &shared_ref<type>::__dummy__;
	}
#else
	return *this->memory;
#endif

}

SHARED_REF_TEMPLATE
const type& shared_ref<type>::operator* () const NOEXP {

#ifdef DEBUG
	if (this->memory) return *this->memory;
	else {
		CORE_ERROR_HPP(_shared_ref_logger, CORE_LOG_CONFIG_ALL, REF_INVALID_MEMORY);
		return (const type&)&shared_ref<type>::__dummy__;
	}
#else
	return C_CAST(*this->memory, const type&);
#endif

}

SHARED_REF_TEMPLATE
shared_ref<type>::operator bool() const NOEXP {
	return this->memory != nullptr;
}


// debug-only functions
#ifdef DEBUG
	SHARED_REF_TEMPLATE 
	DEBUG_ONLY INLINE u32 shared_ref<type>::get_strong_count() NOEXP {
		return this->ctr->__strong__;
	}

	SHARED_REF_TEMPLATE
	DEBUG_ONLY INLINE u32 shared_ref<type>::get_strong_count() const NOEXP {
		return this->ctr->__strong__;
	}

	SHARED_REF_TEMPLATE 
	DEBUG_ONLY INLINE u32 shared_ref<type>::get_weak_count() NOEXP {
		return this->ctr->__weak__;
	}

	SHARED_REF_TEMPLATE 
	DEBUG_ONLY INLINE u32 shared_ref<type>::get_weak_count() const NOEXP {
		return this->ctr->__weak__;
	}
#endif


#endif