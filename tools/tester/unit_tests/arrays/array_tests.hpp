#pragma once 

#ifndef ARRAY_TESTS
#define ARRAY_TESTS

#include "core/data_structres/arrays/array.hpp"
#include "tools/tester/test/test.hpp"

struct DtorCounter {
	public:
		static int count;

		DtorCounter() {};
		DtorCounter(DtorCounter const&) = default;
		DtorCounter& operator= (DtorCounter const&) = default;

		~DtorCounter() { 
			DtorCounter::count++ ; 
		}
};

int DtorCounter::count = 0;

bool array_t_construct_count_size_and_begin_end() {
	core::array<int> a(5);
	if (a.count() != 5u) return false;
	if (a.size() != 5u * sizeof(int)) return false;
	if (a.begin() == a.end()) return false;
	return true;
}

bool array_t_get_set_and_operator_index() {
	core::array<int> a(4);
	a.set(0, 10);
	a.set(1, 20);
	a[2] = 30;
	a[3] = 40;

	if (a.get(0) != 10) return false;
	if (a.get(1) != 20) return false;
	if (a[2] != 30) return false;
	if (a[3] != 40) return false;

	std::vector<int> values(a.begin(), a.end());
	std::vector<int> expected{10,20,30,40};
	if (values != expected) return false;

	return true;
}

bool array_t_clear_and_fill() {
	core::array<int> a(3);
	core::array<int>::fill(a, 7);
	if (a[0] != 7 || a[1] != 7 || a[2] != 7) return false;

	a.clear();
	if (a[0] != 0 || a[1] != 0 || a[2] != 0) return false;

	return true;
}

bool array_t_copy_ctor_and_static_copy() {
	core::array<int> src(3);
	src[0]=1; src[1]=2; src[2]=3;

	core::array<int> copy(src);
	if (copy.count() != src.count()) return false;
	if (copy[0]!=1 || copy[1]!=2 || copy[2]!=3) return false;

	src[1] = 999;
	if (copy[1] != 2) return false;

	core::array<int> dest(0);
	core::array<int>::copy(copy, dest);
	if (dest.count() == copy.count()) return false;
	if (dest[0] == copy[0]) return false;

	return true;
}

bool array_t_static_move_move_assign_move_ctor() {
	core::array<int> src(3);
	src[0]=5; src[1]=6; src[2]=7;

	core::array<int> dest(0);
	core::array<int>::move(src, dest);
	if (dest.count() != 3u) return false;
	if (dest[0]!=5 || dest[1]!=6 || dest[2]!=7) return false;
	if (src.count() != 0u) return false;

	core::array<int> a(3);
	a[0]=10; a[1]=11; a[2]=12;
	core::array<int> b(0);
	b = std::move(a);
	if (b.count() != 3u || b[0]!=10 || b[1]!=11 || b[2]!=12) return false;
	if (a.count() != 0u) return false;

	core::array<int> tmp(2);
	tmp[0]=1; tmp[1]=2;
	core::array<int> m_ctor(std::move(tmp));
	if (m_ctor.count() != 2u || m_ctor[0] != 1 || m_ctor[1] != 2) return false;
	if (tmp.count() == 2u) return false;

	return true;
}

bool array_t_non_trivial_destruction() {

	DtorCounter::count = 0;

	{
		core::array<DtorCounter> arr(4);
	}
	
	if ( DtorCounter::count != 4) return false;

	return true;
}

bool cmp_int(int const& a, int const& b) { 
	return a < b; 
}

bool array_t_sort_function() {
	core::array<int> a(5);
	a[0]=4; a[1]=1; a[2]=5; a[3]=2; a[4]=3;
	core::array<int>::sort(a, &cmp_int);

	std::vector<int> got(a.begin(), a.end());
	std::vector<int> expected{1,2,3,4,5};

	if (got != expected) return false;
	return true;
}

bool array_t_allocate_reallocate() {
	core::array<int> a(2);
	a[0]=9; a[1]=8;
	core::array<int>::reallocate(a, true);
	if (a.count() != 2u) return false;
	volatile int x = a[0]; (void)x;
	return true;
}

bool array_t_fill_edge_cases() {
	core::array<int> a(0);
	core::array<int> b(3);
	core::array<int>::fill(b, -1);
	if (b[0] != -1 || b[1] != -1 || b[2] != -1) return false;
	return true;
}

bool array_t_copy_into_existing_destination() {
	core::array<int> src(5);
	for (u32 i = 0; i < src.count(); ++i) src[i] = static_cast<int>(i+1);
	core::array<int> dest(5);
	core::array<int>::copy(src, dest);
	for (u32 i = 0; i < dest.count(); ++i) if (dest[i] != src[i]) return false;
	return true;
}


// Test basic construction with count (allocates raw memory)
bool array_t_array_basic_construction() {
	core::array<int> arr(5);

	if (arr.count() != 5) return false;
	if (arr.size() != sizeof(int) * 5) return false;
	if (arr.begin() == nullptr) return false;
	if (arr.end() != arr.begin() + 5) return false;

	return true;
}

// Test constructor using raw pointer input (trivial types)
bool array_t_array_constructor_from_pointer_trivial() {
	int data[] = {1, 2, 3, 4};
	core::array<int> arr(data, 4);

	if (arr.count() != 4) return false;
	if (arr[0] != 1 || arr[3] != 4) return false;

	return true;
}

// Test constructor using raw pointer input (non-trivial types, tests placement new)
bool array_t_array_constructor_from_pointer_nontrivial() {
	std::string data[] = {"A", "B", "C"};
	core::array<std::string> arr(data, 3);

	if (arr.count() != 3) return false;
	if (arr[0] != "A" || arr[2] != "C") return false;
	// Check deep copy (ensure original data is distinct)
	data[0] = "Z";
	if (arr[0] == "Z") return false; 

	return true;
}

// Test copy constructor (trivial types)
bool array_t_array_copy_constructor_trivial() {
	core::array<int> original(3);
	original[0] = 99;

	core::array<int> copy = original; // Invokes copy constructor

	if (copy.count() != 3) return false;
	if (copy[0] != 99) return false;
	// Ensure deep copy
	original[0] = 0;
	if (copy[0] == 0) return false;

	return true;
}

// Test copy constructor (non-trivial types, tests placement new)
bool array_t_array_copy_constructor_nontrivial() {
	core::array<std::string> original(2);

	original[0] = "Hello";
	original[1] = "World";
	/*
	original[0] = std::string("Hello");
	original[1] = std::string("world");
	*/

	core::array<std::string> copy = original; // Invokes copy constructor

	if (copy.count() != 2) return false;
	if (copy[0] != "Hello" || copy[1] != "World") return false;
	// Ensure deep copy (original still valid)
	original[0] = "Hi";
	if (copy[0] == "Hi") return false;

	return true;
}

// Test the static copy function for non-trivial types
bool array_t_array_static_copy_nontrivial() {
	core::array<std::string> source(2);
	source[0] = "Source1";
	source[1] = "Source2";

	core::array<std::string> dest(2);
	dest[0] = "Dest1"; // Overwritten by copy

	core::array<std::string>::copy(source, dest); // Test static copy function

	if (dest.count() != 2) return false;
	if (dest[0] != "Source1" || dest[1] != "Source2") return false;

	// Ensure source is untouched
	if (source[0] != "Source1") return false;

	return true;
}

bool array_t_array_clear_function() {
	core::array<int> arr(2);
	arr[0] = 10;
	arr.clear();

	// The clear function uses memset(0) for trivial types
	if (arr[0] != 0 || arr[1] != 0) return false;

	core::array<std::string> arr_str(2);
	arr_str[0] = "Test";
	arr_str.clear(); // Clears using type() default constructor

	if (arr_str[0] != "") return false;

	return true;
}

#endif 
