#pragma once

#include <string>

#include "heap_object.hpp"

namespace anb {

template <typename AllocatorT>
struct string : public heap_object<AllocatorT> {
  string(const AllocatorT& handle) : heap_object<AllocatorT>(handle) {}

  std::string_view view() const { return d_; }

  void set(std::string_view str) { d_ = str; }

  void reset() { set(""); }

  void reset(std::string_view str) { set(str); }

  heap_object_type type() const override { return heap_object_type::string; }

 private:
  std::string d_;
};

}  // namespace anb
