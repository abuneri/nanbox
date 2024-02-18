#pragma once

#include <unordered_map>

#include "heap_object.hpp"
#include "detail/util.hpp"

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

  heap_object_type type() const override { return heap_object_type::dictionary; }

  std::unordered_map<anb::object<AllocatorT>, anb::object<AllocatorT>>
      object_dict_;
};

}  // namespace anb

template <typename AllocatorT>
struct std::hash<
    std::unordered_map<anb::object<AllocatorT>, anb::object<AllocatorT>>> {
  std::size_t operator()(
      const AllocatorT& allocator,
      const std::unordered_map<anb::object<AllocatorT>,
                               anb::object<AllocatorT>>& objects) const {
    std::size_t seed = objects.size();
    for (const auto& [key_obj, val_obj] : objects) {
      seed ^= anb::detail::magic_hash(key_obj.hash());
      seed ^= anb::detail::magic_hash(val_obj.hash());
    }
    return seed;
  }
};

template <typename AllocatorT>
struct std::hash<anb::dictionary<AllocatorT>> {
  std::size_t operator()(const AllocatorT& allocator,
                         const anb::dictionary<AllocatorT>& dict) const {
    return std::hash<
        std::unordered_map<anb::object<AllocatorT>, anb::object<AllocatorT>>>{}(
        allocator, dict.object_dict_);
  }
};
