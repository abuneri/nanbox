#pragma once
#include <anb/object.hpp>

#include <unordered_map>

class mock_allocator {
 public:
  template <template <class> typename HeapObjT>
  HeapObjT<mock_allocator>* alloc() {
    return new HeapObjT<mock_allocator>(*this);
  }

  template <template <class> typename HeapObjT>
  void dealloc(HeapObjT<mock_allocator>* obj_ptr) {
    delete obj_ptr;
  }

  std::unordered_map<void*, anb::heap_object_type> type_lookup;
};
using ma = mock_allocator;

static ma allocator;
