#pragma once

#ifndef DYNAMIC_ARRAY_TESTS
#define DYNAMIC_ARRAY_TESTS

#include "core/data_structres/arrays/dynamic_array.hpp"

// Test basic construction (allocates memory but doesn't initialize objects)
bool t_construction_basics() {
  core::dynamic_array<int> arr(10, 5);  // Capacity 10, resize 5

  if (arr.count() != 10) return false;
  if (arr.size() != sizeof(int) * 10) return false;
  if (arr.begin() == nullptr) return false;

  return true;
}

// Test push and resize with trivial types (int)
bool t_push_and_resize_trivial() {
  core::dynamic_array<int> arr(2, 2);  // Start capacity 2, resize by 2

  // Push 3 elements to force a resize
  arr.push(10);
  arr.push(20);
  if (arr.count() != 2)
    return false;  // Capacity should still be 2 before next push

  arr.push(30);  // Forces a resize

  if (arr.count() != 4) return false;     // New capacity should be 4
  if (arr.get_push_index() != 3) return false;  // Logical size should be 3

  if (arr[0] != 10 || arr[1] != 20 || arr[2] != 30) return false;

  return true;
}

// Test push and resize with non-trivial types (std::string)
// This specifically tests the 'placement new' logic fix
bool t_push_and_resize_nontrivial() {
  core::dynamic_array<std::string> arr(1, 1);  // Start capacity 1, resize by 1

  arr.push("Test1");
  arr.push("Test2");  // Forces resize with std::string
  arr.push("Test3");  // Forces another resize

  if (arr.count() != 3) return false;     // Capacity should be 3
  if (arr.get_push_index() != 3) return false;  // Logical size should be 3

  if (arr[0] != "Test1" || arr[1] != "Test2" || arr[2] != "Test3") return false;

  return true;
}

// Test basic pop functionality with trivial types (int)
bool t_pop_basics_trivial() {
  core::dynamic_array<int> arr(4, 2);
  arr.push(10);
  arr.push(20);

  if (arr.get_push_index() != 2) return false;

  int val = arr.pop(true);  // Pop last element

  if (val != 20) return false;
  if (arr.get_push_index() != 1) return false;

  val = arr.pop(true);
  if (val != 10) return false;
  if (arr.get_push_index() != 0) return false;

  return true;
}

// Test pop functionality with non-trivial types (std::string)
// This specifically tests the destructor calls
bool t_pop_basics_nontrivial() {
  core::dynamic_array<std::string> arr(2, 1);
  arr.push("Hello");
  arr.push("World");

  std::string val =
      arr.pop(true);  // Pop "World", call destructor on internal object

  if (val != "World") return false;
  if (arr.get_push_index() != 1) return false;

  // The remaining element should still be valid
  if (arr[0] != "Hello") return false;

  return true;
}

// Test copy assignment operator with non-trivial types
bool t_copy_assignment_nontrivial() {
  core::dynamic_array<std::string> original(1, 1);
  original.push("A");
  original.push("B");

  core::dynamic_array<std::string> copy(1, 1);
  copy = original;  // Test the operator= we fixed

  if (copy.count() != 2 || copy.get_push_index() != 2) return false;
  if (copy[0] != "A" || copy[1] != "B") return false;

  // Ensure deep copy (original still works independently)
  original.push("C");
  if (copy.get_push_index() == original.get_push_index())
    return false;  // Should be different now

  return true;
}

// Test handling of popping from an empty array
bool t_empty_pop_handling() {
  core::dynamic_array<int> arr(1, 1);
  // int is default constructible, so it should return int() (0)

  int result = arr.pop(true);
  if (result != 0) return false;

  // Test with a non-default constructible type (requires CRASH_IF to work)
  // This part is hard to unit test without external libraries, as CRASH_IF
  // terminates the process. We assume CRASH_IF works as intended if a
  // non-default constructible type is used here.

  return true;
}

#endif