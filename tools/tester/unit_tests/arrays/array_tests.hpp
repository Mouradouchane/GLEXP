#pragma once 

#ifndef ARRAY_TESTS
#define ARRAY_TESTS

#include "core/data_structres/arrays/array.hpp"
#include "tools/tester/test/test.hpp"

static bool t_construct_count_size_and_begin_end() {
	core::array<int> a(5);
	if (a.count() != 5u) return false;
	if (a.size() != 5u * sizeof(int)) return false;
	if (a.begin() == a.end()) return false;
	return true;
}

static bool t_get_set_and_operator_index() {
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

static bool t_clear_and_fill() {
	core::array<int> a(3);
	core::array<int>::fill(a, 7);
	if (a[0] != 7 || a[1] != 7 || a[2] != 7) return false;

	a.clear();
	if (a[0] != 0 || a[1] != 0 || a[2] != 0) return false;

	return true;
}

static bool t_copy_ctor_and_static_copy() {
	core::array<int> src(3);
	src[0]=1; src[1]=2; src[2]=3;

	core::array<int> copy(src);
	if (copy.count() != src.count()) return false;
	if (copy[0]!=1 || copy[1]!=2 || copy[2]!=3) return false;

	src[1] = 999;
	if (copy[1] != 2) return false;

	core::array<int> dest(0);
	core::array<int>::copy(copy, dest);
	if (dest.count() != copy.count()) return false;
	if (dest[0]!=1 || dest[1]!=2 || dest[2]!=3) return false;

	return true;
}

static bool t_static_move_move_assign_move_ctor() {
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
	core::array<int> moved_ctor(std::move(tmp));
	if (moved_ctor.count() != 2u || moved_ctor[0]!=1 || moved_ctor[1]!=2) return false;
	if (tmp.count() != 0u) return false;

	return true;
}

struct DtorCounter {
	static int dtor_count;
	DtorCounter() = default;
	DtorCounter(const DtorCounter&) = default;
	DtorCounter& operator=(const DtorCounter&) = default;
	~DtorCounter() { ++dtor_count; }
};
int DtorCounter::dtor_count = 0;

static bool t_non_trivial_destruction() {
	DtorCounter::dtor_count = 0;
	{
		core::array<DtorCounter> arr(4);
	}
	if (DtorCounter::dtor_count != 4) return false;
	return true;
}

static bool cmp_int(int const& a, int const& b) { 
	return a < b; 
}

static bool t_sort_function() {
	core::array<int> a(5);
	a[0]=4; a[1]=1; a[2]=5; a[3]=2; a[4]=3;
	core::array<int>::sort(a, &cmp_int);

	std::vector<int> got(a.begin(), a.end());
	std::vector<int> expected{1,2,3,4,5};

	if (got != expected) return false;
	return true;
}

static bool t_allocate_reallocate() {
	core::array<int> a(2);
	a[0]=9; a[1]=8;
	core::array<int>::reallocate(a, true);
	if (a.count() != 2u) return false;
	volatile int x = a[0]; (void)x;
	return true;
}

static bool t_fill_edge_cases() {
	core::array<int> a(0);
	core::array<int> b(3);
	core::array<int>::fill(b, -1);
	if (b[0] != -1 || b[1] != -1 || b[2] != -1) return false;
	return true;
}

static bool t_copy_into_existing_destination() {
	core::array<int> src(5);
	for (u32 i = 0; i < src.count(); ++i) src[i] = static_cast<int>(i+1);
	core::array<int> dest(5);
	core::array<int>::copy(src, dest);
	for (u32 i = 0; i < dest.count(); ++i) if (dest[i] != src[i]) return false;
	return true;
}

 
#endif 
