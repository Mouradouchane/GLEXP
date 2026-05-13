#pragma once 

#ifndef CORE_UNIQUE_REF_HPP
#define CORE_UNIQUE_REF_HPP

#include "core/macros.hpp"
#include "core/strings/string.hpp"
#include "core/references/ref_counter.hpp"


/*
	unique_ref for fully ownership by one entity
*/

template<typename type, core::memory::management plan = core::memory::management::by_others>
class unique_ref : private memory_management_plan<plan> {
	requires refcounted_type<type, plan>;
private:
	static inline const STRING type_name = typeid(type).name();
	type* memory = nullptr;

	// not allowed constructors/operators
	unique_ref() = delete;
	unique_ref(unique_ref<type,plan>& other) = delete;
	unique_ref<type,plan>& operator = (unique_ref<type,plan>& other) = delete;

public:

	// constructor's
	unique_ref(type* pointer, core::memory_allocator const& allocator) NOEXP REQUIRE_MEMORY_MANAGEMENT_BY_OTHERS;
	unique_ref(type* pointer) NOEXP REQUIRE_MEMORY_MANAGEMENT_BY_IT_SELF;
	unique_ref(unique_ref<type,plan>&& other) NOEXP;

	// destructor
	~unique_ref( ) NOEXP;

	// operator's
	unique_ref<type,plan>& operator = (type* pointer) NOEXP REQUIRE_MEMORY_MANAGEMENT_BY_IT_SELF;
	unique_ref<type,plan>& operator = (unique_ref<type,plan>&& other) NOEXP;

	type* operator->() NOEXP;
	type& operator*()  NOEXP;
	const type& operator*()  const NOEXP;

	operator bool() const NOEXP;

	// public function's
	type* pass_ownership() NOEXP REQUIRE_MEMORY_MANAGEMENT_BY_IT_SELF;

	static bool move_ownership(unique_ref<type,plan>& ref , unique_ref<type,plan>& new_owner);

private :

	// friend factory function 
	template<typename type, core::memory::management plan, typename... parameters>
	requires refcounted_type<type, plan>
	
	friend unique_ref<type, plan> core::make::unique_ref_(core::memory_allocator const& allocator, parameters&&... constructor_parameters) NOEXP;

}; 
// class unique_ref end


namespace core {

	namespace make {

		template<typename type, core::memory::management plan, typename... parameters>
		requires refcounted_type<type, plan>

		unique_ref<type,plan> unique_ref_(core::memory_allocator const& allocator, parameters&&... constructor_parameters) NOEXP;


	} // namespace make end

} // namespace core end


#include "unique_ref_impl.hpp"

#endif
