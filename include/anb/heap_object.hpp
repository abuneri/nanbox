#pragma once

namespace anb {

enum class heap_object_type { string, list, dictionary };
template <typename AllocatorT> struct heap_object {
  heap_object(AllocatorT &handle) : allocator_handle(handle) {}
  virtual ~heap_object() = default;

  virtual heap_object_type type() const = 0;

  AllocatorT &allocator_handle;
};

} // namespace anb
