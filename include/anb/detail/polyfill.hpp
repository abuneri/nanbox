#pragma once

#include <functional>
#include <string>
#include <array>

#include "util.hpp"

namespace anb::detail {

//=====================================================================
// https://en.cppreference.com/w/cpp/utility/variant/visit
// helper type for the visitor #4
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};

//=====================================================================
// https://en.cppreference.com/w/cpp/experimental/scope_exit
class scope_guard {
 public:
  scope_guard(const scope_guard &) = delete;
  scope_guard &operator=(const scope_guard &) = delete;
  scope_guard(scope_guard &&) = default;
  scope_guard &operator=(scope_guard &&) = default;
  ~scope_guard() { scope_exit_callback_(); }

  template <typename Func>
  static auto make(Func &&f) {
    scope_guard guard;
    guard.scope_exit_callback_ = f;
    return guard;
  }

 private:
  scope_guard() = default;

  std::function<void()> scope_exit_callback_;
};

//=====================================================================
// https://en.cppreference.com/w/cpp/utility/unreachable
[[noreturn]] inline void unreachable() {
#if defined(_MSC_VER) && !defined(__clang__)  // MSVC
  __assume(false);
#else  // GCC, Clang
  __builtin_unreachable();
#endif
}

//=====================================================================
// https://www.open-std.org/JTC1/SC22/WG21/docs/papers/2016/p0259r0.pdf
template <std::size_t N>
struct fixed_string {
  constexpr fixed_string(const char (&str)[N]) { std::copy_n(str, N, data); }

  char data[N];
  std::size_t size{N};
};

template <fixed_string str>
std::string from_fixed_string() {
  constexpr auto data = str.data;
  constexpr auto size = str.size;
  return std::string{data, size};
}

//=====================================================================
// https://www.open-std.org/JTC1/SC22/WG21/docs/papers/2018/p0843r2.html
// Implementation directly taken (MIT License) from Jason Turner (lefticus):
//  https://github.com/lefticus/tools/blob/main/include/lefticus/tools/simple_stack_vector.hpp
//
//  changes from std::vector
//      * capacity if fixed at compile-time
//      * it never allocates
//      * items must be default constructible
//      * items are never destroyed until the entire stack_vector
//          is destroyed.
//      * iterators are never invalidated
//      * capacity() and max_size() are now static functions
//      * should be fully C++17 usable within constexpr
//
//  changes from abuneri:
//      * runtime assertion checks instead of throwing
//      * added data() method to access underlying memory address required to
//          send to executable
template <typename Contained, std::size_t Capacity>
struct static_vector {
  using value_type = Contained;
  using data_type = std::array<value_type, Capacity>;
  using size_type = typename data_type::size_type;
  using difference_type = typename data_type::difference_type;
  using reference = value_type &;
  using const_reference = const value_type &;

  static_assert(std::is_default_constructible_v<Contained>);

  using iterator = typename data_type::iterator;
  using const_iterator = typename data_type::const_iterator;
  using reverse_iterator = typename data_type::reverse_iterator;
  using const_reverse_iterator = typename data_type::const_reverse_iterator;

  constexpr static_vector() = default;
  constexpr explicit static_vector(std::initializer_list<value_type> values) {
    for (const auto &value : values) {
      push_back(value);
    }
  }

  template <typename OtherContained, std::size_t OtherSize>
  constexpr explicit static_vector(
      const static_vector<OtherContained, OtherSize> &other) {
    for (const auto &value : other) {
      push_back(Contained{value});
    }
  }

  template <typename Type>
  constexpr explicit static_vector(const std::vector<Type> &values) {
    for (const auto &value : values) {
      push_back(Contained{value});
    }
  }

  template <typename Itr>
  constexpr static_vector(Itr begin, Itr end) {
    while (begin != end) {
      push_back(*begin);
      ++begin;
    }
  }

  [[nodiscard]] constexpr iterator begin() noexcept { return data_.begin(); }

  [[nodiscard]] constexpr const_iterator begin() const noexcept {
    return data_.cbegin();
  }
  [[nodiscard]] constexpr const_iterator cbegin() const noexcept {
    return data_.cbegin();
  }

  [[nodiscard]] constexpr iterator end() noexcept {
    return std::next(data_.begin(), static_cast<difference_type>(size_));
  }

  [[nodiscard]] constexpr const_iterator end() const noexcept {
    return std::next(data_.cbegin(), static_cast<difference_type>(size_));
  }

  [[nodiscard]] constexpr value_type &front() noexcept { return data_.front(); }
  [[nodiscard]] constexpr const value_type &front() const noexcept {
    return data_.front();
  }
  [[nodiscard]] constexpr value_type &back() noexcept { return data_.back(); }
  [[nodiscard]] constexpr const value_type &back() const noexcept {
    return data_.back();
  }

  [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }

  [[nodiscard]] constexpr reverse_iterator rbegin() noexcept {
    return std::next(data_.rbegin(),
                     static_cast<difference_type>(Capacity - size_));
  }

  [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept {
    return std::next(data_.crbegin(),
                     static_cast<difference_type>(Capacity - size_));
  }
  [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept {
    return rbegin();
  }

  [[nodiscard]] value_type *data() noexcept { return data_.data(); }

  [[nodiscard]] const value_type *data() const { return data_.data(); }

  [[nodiscard]] constexpr bool empty() const noexcept { return size_ == 0; }

  [[nodiscard]] constexpr reverse_iterator rend() noexcept {
    return data_.rend();
  }

  [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept {
    return data_.crend();
  }

  [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept {
    return data_.crend();
  }

  template <typename Value>
  constexpr value_type &push_back(Value &&value) {
    AVM_ASSERT(size_ < Capacity, "push_back would exceed static capacity");
    data_[size_] = std::forward<Value>(value);
    return data_[size_++];
  }

  template <typename... Param>
  constexpr value_type &emplace_back(Param &&...param) {
    AVM_ASSERT(size_ < Capacity, "emplace_back would exceed static capacity");
    data_[size_] = value_type{std::forward<Param>(param)...};
    return data_[size_++];
  }

  [[nodiscard]] constexpr value_type &operator[](
      const std::size_t idx) noexcept {
    return data_[idx];
  }

  [[nodiscard]] constexpr const value_type &operator[](
      const std::size_t idx) const noexcept {
    return data_[idx];
  }

  [[nodiscard]] constexpr value_type &at(const std::size_t idx) {
    AVM_ASSERT(idx < size_, "index past end of stack_vector");
    return data_[idx];
  }

  [[nodiscard]] constexpr const value_type &at(const std::size_t idx) const {
    AVM_ASSERT(idx < size_, "index past end of stack_vector");
    return data_[idx];
  }

  // resets the size to 0, but does not destroy any existing objects
  constexpr void clear() { size_ = 0; }

  // cppcheck-suppress functionStatic
  constexpr void reserve(size_type new_capacity) {
    AVM_ASSERT(new_capacity < Capacity,
               "new capacity would exceed max_size for stack_vector");
  }

  // cppcheck-suppress functionStatic
  [[nodiscard]] constexpr static size_type capacity() noexcept {
    return Capacity;
  }

  // cppcheck-suppress functionStatic
  [[nodiscard]] constexpr static size_type max_size() noexcept {
    return Capacity;
  }

  [[nodiscard]] constexpr size_type size() const noexcept { return size_; }

  constexpr void resize(const size_type new_size) {
    if (new_size <= size_) {
      size_ = new_size;
    } else {
      AVM_ASSERT(new_size < Capacity, "resize would exceed static capacity");
      auto old_end = end();
      size_ = new_size;
      auto new_end = end();
      while (old_end != new_end) {
        *old_end = data_type{};
        ++old_end;
      }
    }
  }

  constexpr void pop_back() noexcept { --size_; }

  // cppcheck-suppress functionStatic
  constexpr void shrink_to_fit() noexcept {
    // nothing to do here
  }

 private:
  // default initializing to make it more C++17 friendly
  data_type data_{};
  size_type size_{};
};

template <typename Contained, std::size_t LHSSize, std::size_t RHSSize>
[[nodiscard]] constexpr bool operator==(
    const static_vector<Contained, LHSSize> &lhs,
    const static_vector<Contained, RHSSize> &rhs) {
  if (lhs.size() == rhs.size()) {
    for (std::size_t idx = 0; idx < lhs.size(); ++idx) {
      if (lhs[idx] != rhs[idx]) {
        return false;
      }
    }
    return true;
  }

  return false;
}

}  // namespace anb::detail
