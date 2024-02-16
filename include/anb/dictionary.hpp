#pragma once

#include <unordered_map>

#include "heap_object.hpp"

namespace anb {

template <typename AllocatorT>
class object;

template <typename AllocatorT>
struct dictionary : public heap_object<AllocatorT> {
  dictionary(const AllocatorT& handle) : heap_object<AllocatorT>(handle) {}

  template <template <class, class> typename... ArgPairs>
  void set(
      ArgPairs<anb::object<AllocatorT>, anb::object<AllocatorT>>&&... args) {
    static_assert(
        (std::is_constructible_v<
             std::pair<anb::object<AllocatorT>, anb::object<AllocatorT>>,
             ArgPairs<anb::object<AllocatorT>, anb::object<AllocatorT>>&&> &&
         ...));
    (object_dict_.insert(
         std::forward<
             ArgPairs<anb::object<AllocatorT>, anb::object<AllocatorT>>>(args)),
     ...);
  }

  void reset() { object_dict_.clear(); }

  template <template <class, class> typename... ArgPairs>
  void reset(
      ArgPairs<anb::object<AllocatorT>, anb::object<AllocatorT>>&&... args) {
    reset();
    set(std::forward<
        ArgPairs<anb::object<AllocatorT>, anb::object<AllocatorT>>>(args)...);
  }

  heap_object_type type() const { return heap_object_type::dictionary; }

  std::unordered_map<anb::object<AllocatorT>, anb::object<AllocatorT>>
      object_dict_;
};

}  // namespace anb
