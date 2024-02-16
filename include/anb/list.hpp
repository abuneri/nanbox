#pragma once

#include <vector>

#include "heap_object.hpp"

namespace anb {

template <typename AllocatorT>
class object;

template <typename AllocatorT>
struct list : public heap_object<AllocatorT> {
  list(const AllocatorT& handle) : heap_object<AllocatorT>(handle) {}

  template <typename... Args>
  void set(Args&&... args) {
    static_assert(
        (std::is_constructible_v<anb::object<AllocatorT>, Args&&> && ...));
    (objects_.push_back(std::forward<Args>(args)), ...);
  }

  void reset() { objects_.clear(); }

  template <typename... Args>
  void reset(Args&&... args) {
    reset();
    set(std::forward<Args>(args)...);
  }

  heap_object_type type() const { return heap_object_type::list; }

  std::vector<anb::object<AllocatorT>> objects_;
};
}  // namespace anb
