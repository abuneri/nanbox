# (A)buneris (N)an(b)ox: Dynamic Typing
A dynamically typed and space efficient object implementation

## Supported Fixed Types
- Null/None/Nothing
- qNaN
- Boolean
- Signed 32-bit Integer
- 64-bit Floating Point
- String (SSO)

## Supported Heap Types
- String
- List
- Dictionary

### Heap Customization
The allocator is fully customizable as long as it meets the general API requirements

## Installation
### Build and install project

```
mkdir build
cd build

# Without tests
cmake .. -DBUILD_TESTING=OFF
cmake --build . --config Release

# With tests
cmake ..
cmake --build . --config Release
ctest

cmake --install . --config Release
```

### Include package in your own CMake projects

```
# Your projects CMakeLists.txt

find_package(anb REQUIRED)

# ... configure <your_target> ...

target_link_libraries(<your_target>
    anb
)
```

## Example
```cpp
#include <anb/object.hpp>

#include <memory>
#include <string>
#include <unordered_map>

// Implement required allocator interface, only used when object is set to one of the heap types
class my_allocator {
 public:
  template <template <class> typename HeapObjT>
  HeapObjT<my_allocator>* alloc() {
    return new HeapObjT<mock_allocator>(*this);
  }

  template <template <class> typename HeapObjT>
  void dealloc(HeapObjT<my_allocator>* obj_ptr) {
    delete obj_ptr;
  }

  std::unordered_map<void*, anb::heap_object_type> type_lookup;
};
using ma = my_allocator;
ma g_allocator;

// Fixed types
anb::object<ma> i32(static_cast<std::int32_t>(0xDEADBEEF));
if (i32.as_int32() == 0xDEADBEEF) {
    // ...
}

// Heap types
auto heap_str = anb::object<ma>::make_string_heap(g_allocator);

anb::string<ma>& str_data = heap_str.as_string_heap(g_allocator);

const std::string lorem =
  "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
  "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad "
  "minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip "
  "ex ea commodo consequat. Duis aute irure dolor in reprehenderit in "
  "voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur "
  "sint occaecat cupidatat non proident, sunt in culpa qui officia "
  "deserunt mollit anim id est laborum.";
str_data.set(lorem);

if (heap_str.is_heap_string(g_allocator)) {
    // ...
}
```
