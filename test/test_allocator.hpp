#pragma once

#include <algorithm>
#include <anb/heap_object.hpp>
#include <vector>

class mock_allocator {
 public:
  template <template <class> typename HeapObjT>
  HeapObjT<mock_allocator>* alloc() {
    auto ptr = new HeapObjT<mock_allocator>(*this);
    allocated_objects_.push_back(ptr);
    return ptr;
  }

  template <template <class> typename HeapObjT>
  void dealloc(HeapObjT<mock_allocator>* obj_ptr) {
    allocated_objects_.erase(std::remove(allocated_objects_.begin(),
                                         allocated_objects_.end(), obj_ptr));
    delete obj_ptr;
  }

  void pop() {
    if (auto obj_ptr = static_cast<anb::heap_object<mock_allocator>*>(
            allocated_objects_.back())) {
      dealloc(obj_ptr);
    }
  }

  std::vector<void*> allocated_objects_;
};
using ma = mock_allocator;

static ma allocator;
