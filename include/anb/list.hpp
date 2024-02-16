#pragma once

#include <vector>

#include "heap_object.hpp"
#include "detail/util.hpp"

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

template <typename AllocatorT>
struct std::hash<std::vector<anb::object<AllocatorT>>> {
  std::size_t operator()(
      const AllocatorT& allocator,
      const std::vector<anb::object<AllocatorT>>& objects) const {
    std::size_t seed = objects.size();
    for (const auto& obj : objects) {
      seed ^= anb::detail::magic_hash(obj.hash());
    }
    return seed;
  }
};

template <typename AllocatorT>
struct std::hash<anb::list<AllocatorT>> {
  std::size_t operator()(const AllocatorT& allocator,
                         const anb::list<AllocatorT>& list) const {
    return std::hash<std::vector<anb::object<AllocatorT>>>{}(allocator,
                                                             list.objects_);
  }
};
