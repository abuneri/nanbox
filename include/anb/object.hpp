#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "detail/nanbox.hpp"
#include "detail/polyfill.hpp"
#include "dictionary.hpp"
#include "list.hpp"
#include "string.hpp"

namespace anb {

// TODO: make an allocator concept that captures current specified API
// TODO: make specifying allocator optional (default to a null allocator).
// creating heap based objects will fail
template <typename AllocatorT>
class object {
 public:
  // TODO: default ctor should be the 'nothing' type
  object() : value_(0.0) {}

  static object make_qnan() {
    object o;
    o.value_ = *reinterpret_cast<const double*>(
        &detail::nanbox::fixed_type_qnan_value);
    return o;
  }

  static object make_nothing() {
    object o;
    o.value_ = *reinterpret_cast<const double*>(
        &detail::nanbox::fixed_type_null_value);
    return o;
  }

  explicit object(const bool bool_val) {
    value_ = *reinterpret_cast<const double*>(
        &(bool_val ? detail::nanbox::fixed_type_true_value
                   : detail::nanbox::fixed_type_false_value));
  }

  explicit object(const std::int32_t int32_val) {
    const std::uint64_t nb_int_data =
        (*reinterpret_cast<const std::uint64_t*>(&int32_val) &
         detail::nanbox::fixed_type_int32_data_mask);
    const std::uint64_t nb_int =
        detail::nanbox::fixed_type_int32_value | nb_int_data;
    value_ = *reinterpret_cast<const double*>(&nb_int);
  }

  explicit object(const double fp64_val) { value_ = fp64_val; }

  explicit object(const std::string_view str_val) {
    ANB_ASSERT(str_val.size() <= max_sso_len,
               "String size too large for non-heap allocated nanbox");
    value_ = make_string_sso(str_val);
  }

  template <template <class> typename HeapObjT>
  static object alloc_heap(AllocatorT& allocator) {
    object o;

    HeapObjT<AllocatorT>* heap_ptr = allocator.template alloc<HeapObjT>();
    allocator.type_lookup[heap_ptr] = heap_ptr->type();

    const std::uint64_t nb_heap_ptr = detail::nanbox::heap_type_value |
                                      reinterpret_cast<std::uint64_t>(heap_ptr);
    return object{*reinterpret_cast<const double*>(&nb_heap_ptr)};
  }

  template <template <class> typename HeapObjT>
  static void dealloc_heap(object& obj, AllocatorT& allocator) {
    const std::uint64_t nb_heap =
        *reinterpret_cast<const std::uint64_t*>(&obj.value_);
    const std::uint64_t nb_data_pointer =
        nb_heap & detail::nanbox::heap_type_data_mask;
    if (auto heap_ptr = obj.get_heap_ptr<HeapObjT>()) {
      allocator.type_lookup.erase(heap_ptr);
      allocator.template dealloc<HeapObjT>(heap_ptr);

      // TODO: add proper handling for nullptr heap objects
      obj.value_ =
          *reinterpret_cast<const double*>(&detail::nanbox::heap_type_value);
    }
  }

  template <template <class> typename HeapObjT>
  HeapObjT<AllocatorT>* get_heap_ptr() const {
    const std::uint64_t nb_val = as_nb();
    if ((nb_val & detail::nanbox::heap_type_value) ==
        detail::nanbox::heap_type_value) {
      const std::uint64_t nb_data_pointer =
          nb_val & detail::nanbox::heap_type_data_mask;
      return reinterpret_cast<HeapObjT<AllocatorT>*>(nb_data_pointer);
    }
    return nullptr;
  }

  void dealloc_heap(AllocatorT& allocator) {
    if (is_heap_string(allocator)) {
      object::dealloc_heap<string>(*this, allocator);
    } else if (is_list(allocator)) {
      object::dealloc_heap<list>(*this, allocator);
    } else if (is_dictionary(allocator)) {
      object::dealloc_heap<dictionary>(*this, allocator);
    }
  }

  static object make_string_heap(AllocatorT& allocator) {
    return alloc_heap<string>(allocator);
  }

  static object make_list(AllocatorT& allocator) {
    return alloc_heap<list>(allocator);
  }

  static object make_dictionary(AllocatorT& allocator) {
    return alloc_heap<dictionary>(allocator);
  }

  // TODO: make_tree()
  // TODO: make_graph()

  // TODO: impl to_<type> conversions

  bool is_qnan() const {
    return ((as_nb() & detail::nanbox::signature_mask) ==
            detail::nanbox::fixed_type_qnan_value);
  }

  double as_qnan() const {
    ANB_ASSERT(is_qnan(), "Underlying object is not a qnan");
    return value_;
  }

  bool is_boolean() const {
    return ((as_nb() & detail::nanbox::signature_mask) ==
            detail::nanbox::fixed_type_false_value) ||
           ((as_nb() & detail::nanbox::signature_mask) ==
            detail::nanbox::fixed_type_true_value);
  }

  bool as_boolean() const {
    ANB_ASSERT(is_boolean(), "Underlying object is not a boolean");
    return !(as_nb() & detail::nanbox::fixed_type_false_mask);
  }

  bool is_nothing() const {
    return ((as_nb() & detail::nanbox::signature_mask) ==
            detail::nanbox::fixed_type_null_value);
  }

  bool is_int32() const {
    return ((as_nb() & detail::nanbox::signature_mask) ==
            detail::nanbox::fixed_type_int32_value);
  }

  std::int32_t as_int32() const {
    ANB_ASSERT(is_int32(), "Underlying object is not an int32");

    return *reinterpret_cast<const std::uint64_t*>(&value_) &
           detail::nanbox::fixed_type_int32_data_mask;
  }

  bool is_float64() const {
    const std::uint64_t exp_value =
        as_nb() & ~detail::nanbox::sign_mask & ~(0xFFFFFFFFFFFFF);

    return (exp_value != detail::nanbox::nan_exponent_mask);
  }

  double as_float64() const {
    ANB_ASSERT(is_float64(), "Underlying object is not a float64");
    return value_;
  }

  bool is_sso_string() const {
    return ((as_nb() & detail::nanbox::fixed_type_packed_string_mask) ==
            detail::nanbox::fixed_type_packed_string_mask) ||
           ((as_nb() & detail::nanbox::fixed_type_nonpacked_string_mask) ==
            detail::nanbox::fixed_type_nonpacked_string_mask);
  }

  std::string as_string_sso() const {
    ANB_ASSERT(is_sso_string(), "Underlying object is not fixed size string");

    const std::uint64_t nb_val = as_nb();

    const bool is_sso_packed =
        ((nb_val & detail::nanbox::fixed_type_packed_string_mask) ==
         detail::nanbox::fixed_type_packed_string_mask);
    if (is_sso_packed) {
      return as_string_packed_sso(nb_val);
    }

    const bool is_sso_nonpacked =
        ((nb_val & detail::nanbox::fixed_type_nonpacked_string_mask) ==
         detail::nanbox::fixed_type_nonpacked_string_mask);
    if (is_sso_nonpacked) {
      return as_string_nonpacked_sso(nb_val);
    }

    detail::unreachable();
  }

  bool is_heap_string(const AllocatorT& allocator) const {
    if (const auto heap_ptr = get_heap_ptr<string>()) {
      auto type_lookup_itr = allocator.type_lookup.find(heap_ptr);
      if (type_lookup_itr != allocator.type_lookup.end()) {
        return (type_lookup_itr->second == heap_object_type::string);
      }
    }
    return false;
  }

  string<AllocatorT>& as_string_heap(const AllocatorT& allocator) const {
    ANB_ASSERT(is_heap_string(allocator),
               "Underlying object is not a heap allocated string");

    return deref_heap_obj<string<AllocatorT>>();
  }

  bool is_list(const AllocatorT& allocator) const {
    if (const auto heap_ptr = get_heap_ptr<list>()) {
      auto type_lookup_itr = allocator.type_lookup.find(heap_ptr);
      if (type_lookup_itr != allocator.type_lookup.end()) {
        return (type_lookup_itr->second == heap_object_type::list);
      }
    }
    return false;
  }

  list<AllocatorT>& as_list(const AllocatorT& allocator) const {
    ANB_ASSERT(is_list(allocator),
               "Underlying object is not a heap allocated list");
    return deref_heap_obj<list<AllocatorT>>();
  }

  bool is_dictionary(const AllocatorT& allocator) const {
    if (const auto heap_ptr = get_heap_ptr<dictionary>()) {
      auto type_lookup_itr = allocator.type_lookup.find(heap_ptr);
      if (type_lookup_itr != allocator.type_lookup.end()) {
        return (type_lookup_itr->second == heap_object_type::dictionary);
      }
    }
    return false;
  }

  dictionary<AllocatorT>& as_dictionary(const AllocatorT& allocator) const {
    ANB_ASSERT(is_dictionary(allocator),
               "Underlying object is not a heap allocated dictionary");
    return deref_heap_obj<dictionary<AllocatorT>>();
  }

  // TODO: is_tree()
  // TODO: as_tree()

  // TODO: is_graph()
  // TODO: as_graph()

  // TODO: implement assignment operators

  std::size_t hash() const {
    if (is_heap()) {
      return heap_hash();
    }
    return fixed_hash();
  }

  // TODO: this shouldn't be exposed
  std::uint64_t nanbox_value() const { return as_nb(); }

 private:
  std::uint64_t as_nb() const {
    return *reinterpret_cast<const std::uint64_t*>(&value_);
  }

  bool is_heap() const {
    const std::uint64_t nb_val = as_nb();
    const std::uint64_t heap_val = nb_val & detail::nanbox::signature_mask &
                                   ~detail::nanbox::heap_type_data_mask;
    return (heap_val == detail::nanbox::heap_type_value);
  }

  static std::uint64_t to_sso_nb_val(const std::string_view str_val,
                                     const std::size_t str_index,
                                     const std::size_t shift_amount) {
    return detail::lshift(static_cast<unsigned char>(str_val[str_index]),
                          shift_amount);
  }

  static double make_string_sso(const std::string_view str_val) {
    object o;

    const std::size_t str_len = str_val.size();
    if (str_len == max_sso_len) {
      const auto t = str_val[0];
      const std::uint64_t nb_p_str =
          detail::nanbox::fixed_type_packed_string_value |
          to_sso_nb_val(str_val, 5, 40) | to_sso_nb_val(str_val, 4, 32) |
          to_sso_nb_val(str_val, 3, 24) | to_sso_nb_val(str_val, 2, 16) |
          to_sso_nb_val(str_val, 1, 8) | to_sso_nb_val(str_val, 0, 0);
      return *reinterpret_cast<const double*>(&nb_p_str);
    } else {
      std::uint64_t nb_np_str =
          detail::nanbox::fixed_type_nonpacked_string_value;
      nb_np_str |= detail::lshift(str_len, 40);
      switch (str_len) {
        case 5:
          nb_np_str |= to_sso_nb_val(str_val, 4, 32);
          nb_np_str |= to_sso_nb_val(str_val, 3, 24);
          nb_np_str |= to_sso_nb_val(str_val, 2, 16);
          nb_np_str |= to_sso_nb_val(str_val, 1, 8);
          nb_np_str |= to_sso_nb_val(str_val, 0, 0);
          break;
        case 4:
          nb_np_str |= to_sso_nb_val(str_val, 3, 32);
          nb_np_str |= to_sso_nb_val(str_val, 2, 24);
          nb_np_str |= to_sso_nb_val(str_val, 1, 16);
          nb_np_str |= to_sso_nb_val(str_val, 0, 8);
          break;
        case 3:
          nb_np_str |= to_sso_nb_val(str_val, 2, 32);
          nb_np_str |= to_sso_nb_val(str_val, 1, 24);
          nb_np_str |= to_sso_nb_val(str_val, 0, 16);
          break;
        case 2:
          nb_np_str |= to_sso_nb_val(str_val, 1, 32);
          nb_np_str |= to_sso_nb_val(str_val, 0, 24);
          break;
        case 1:
          nb_np_str |= to_sso_nb_val(str_val, 0, 32);
          break;
        case 0:
          break;
        default:
          break;
      }
      return *reinterpret_cast<const double*>(&nb_np_str);
    }
  }

  static char to_sso_char(const std::uint64_t nb_val,
                          const std::size_t shift_amount) {
    return static_cast<char>(
        ((nb_val & detail::nanbox::fixed_type_sso_string_data_mask) >>
         shift_amount) &
        0xFF);
  };

  static std::string as_string_packed_sso(const std::uint64_t nb_val) {
    return std::string{to_sso_char(nb_val, 0),  to_sso_char(nb_val, 8),
                       to_sso_char(nb_val, 16), to_sso_char(nb_val, 24),
                       to_sso_char(nb_val, 32), to_sso_char(nb_val, 40)};
  }

  static std::string as_string_nonpacked_sso(const std::uint64_t nb_val) {
    const std::size_t str_len =
        ((nb_val & detail::nanbox::fixed_type_sso_string_data_mask) >> 40) &
        0xFF;

    std::string str;
    str.reserve(str_len);

    auto to_char = [&nb_val](const std::size_t shift_amount) {

    };

    switch (str_len) {
      case 5:
        str.push_back(to_sso_char(nb_val, 0));
        str.push_back(to_sso_char(nb_val, 8));
        str.push_back(to_sso_char(nb_val, 16));
        str.push_back(to_sso_char(nb_val, 24));
        str.push_back(to_sso_char(nb_val, 32));
        break;
      case 4:
        str.push_back(to_sso_char(nb_val, 8));
        str.push_back(to_sso_char(nb_val, 16));
        str.push_back(to_sso_char(nb_val, 24));
        str.push_back(to_sso_char(nb_val, 32));
        break;
      case 3:
        str.push_back(to_sso_char(nb_val, 16));
        str.push_back(to_sso_char(nb_val, 24));
        str.push_back(to_sso_char(nb_val, 32));
        break;
      case 2:
        str.push_back(to_sso_char(nb_val, 24));
        str.push_back(to_sso_char(nb_val, 32));
        break;
      case 1:
        str.push_back(to_sso_char(nb_val, 32));
        break;
      case 0:
        break;
      default:
        break;
    }
    return str;
  }

  template <typename HeapObjT>
  HeapObjT& deref_heap_obj() const {
    const std::uint64_t nb_heap =
        *reinterpret_cast<const std::uint64_t*>(&value_);
    const std::uint64_t nb_data_pointer =
        nb_heap & detail::nanbox::heap_type_data_mask;

    if (is_heap() && nb_heap & detail::nanbox::sign_mask) {
      return *reinterpret_cast<HeapObjT*>(nb_data_pointer);
    }

    detail::unreachable();
  }

  std::size_t fixed_hash() const {
    if (is_qnan() || is_nothing() || is_float64()) {
      return std::hash<double>{}(value_);
    } else if (is_boolean()) {
      return std::hash<bool>{}(as_boolean());
    } else if (is_int32()) {
      return std::hash<std::int32_t>{}(as_int32());
    } else if (is_sso_string()) {
      return std::hash<std::string>{}(as_string_sso());
    }

    detail::unreachable();
  }

  std::size_t heap_hash() const {
    const AllocatorT& allocator =
        deref_heap_obj<heap_object<AllocatorT>>().allocator_handle;
    if (is_heap_string(allocator)) {
      return std::hash<std::string_view>{}(as_string_heap(allocator).view());
    } else if (is_list(allocator)) {
      return std::hash<list<AllocatorT>>{}(allocator, as_list(allocator));
    } else if (is_dictionary(allocator)) {
      return std::hash<dictionary<AllocatorT>>{}(allocator,
                                                 as_dictionary(allocator));
    }
    detail::unreachable();
  }

  inline static constexpr std::size_t max_sso_len = 6;

  double value_;
};

// TODO: implement less than comparison for sorting

template <typename AllocatorT>
inline bool operator==(const anb::object<AllocatorT>& lhs,
                       const anb::object<AllocatorT>& rhs) {
  return (lhs.hash() == rhs.hash());
}

template <typename AllocatorT>
inline bool operator!=(const anb::object<AllocatorT>& lhs,
                       const anb::object<AllocatorT>& rhs) {
  return !(lhs == rhs);
}

}  // namespace anb

template <typename AllocatorT>
struct std::hash<anb::object<AllocatorT>> {
  std::size_t operator()(const anb::object<AllocatorT>& obj) const {
    return obj.hash();
  }
};
