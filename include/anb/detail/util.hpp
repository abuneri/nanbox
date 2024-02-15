#pragma once

#include <cstdint>
#include <iostream>

namespace anb::detail {

//=====================================================================
constexpr std::uint64_t lshift(std::uint64_t val, int amount) {
  return val << amount;
}

//=====================================================================
// https://stackoverflow.com/questions/3767869/adding-message-to-assert
#ifndef NDEBUG
#define AVM_ASSERT(condition, message)                                   \
  do {                                                                   \
    if (!(condition)) {                                                  \
      std::cerr << "Assertion `" #condition "` failed in " << __FILE__   \
                << " line " << __LINE__ << ": " << message << std::endl; \
      std::terminate();                                                  \
    }                                                                    \
  } while (false)
#else
#define AVM_ASSERT(condition, message) \
  do {                                 \
  } while (false)
#endif

//=====================================================================
// Credits to the hashing wizards
// https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key/12996028#12996028
// NOTE: This algorithm is assuming 64-bit, since the virtual
// machine only plans on supporting 64-bit platforms
inline std::size_t magic_hash(std::size_t x) {
  x = (x ^ (x >> 30)) * std::uint_least64_t(0xbf58476d1ce4e5b9);
  x = (x ^ (x >> 27)) * std::uint_least64_t(0x94d049bb133111eb);
  x = x ^ (x >> 31);
  return x;
}

}  // namespace anb::detail
