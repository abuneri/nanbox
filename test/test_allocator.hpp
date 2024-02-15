#pragma once
#include <anb/object.hpp>
#include <unordered_map>

class mock_allocator {
 public:
  template <template <class> typename HeapObjT>
  HeapObjT<mock_allocator>* alloc() {
    void* heap_ptr = std::malloc(sizeof(HeapObjT<mock_allocator>));
    HeapObjT<mock_allocator>* obj_ptr = std::construct_at(
        reinterpret_cast<HeapObjT<mock_allocator>*>(heap_ptr), *this);
    return obj_ptr;
  }

  template <template <class> typename HeapObjT>
  void dealloc(HeapObjT<mock_allocator>* obj_ptr) {
    obj_ptr->~HeapObjT<mock_allocator>();
  }

  std::unordered_map<void*, anb::heap_object_type> type_lookup;
};
using ma = mock_allocator;

static ma allocator;
