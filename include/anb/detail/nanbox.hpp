#pragma once
#include <cstdint>

#include "util.hpp"

namespace anb::detail {

struct nanbox {
// clang-format off
//=====================================================================
// Core floating point properties
//
//	+- Sign bit
//	|+- 11 Exponent bits
//	||          +- 52 Mantissa bits
//	vv          v
//	S[Exponent-][Mantissa------------------------------------------]
//=====================================================================
inline static constexpr std::uint64_t sign_mask = lshift(1, 63);
inline static constexpr std::uint64_t nan_exponent_mask = lshift(0x7FF, 52);

//=====================================================================
// NaN type properties (qNaN vs sNaN)
//
// -11111111111Q---------------------------------------------------
//             ^
//             +- Signalling / Quiet bit
//=====================================================================
inline static constexpr std::uint64_t nan_quiet_mask = lshift(1, 51);

//=====================================================================
// NaN box fixed types definition (value types)
//
// +- Sign bit set to 0
// |+- Exponent bits all set to 1
// ||
// ||          +- Quiet bit
// ||          |
// ||          |+- Type ID set to 100, meaning Integer
// vv          vv
// 0[Exponent ]1100IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
//                 ^
//                 +- 48 bits left to store an integer.
//
// Type IDs:
//      000 -> NaN
//      001 -> false (boolean)
//      010 -> true (boolean)
//      011 -> null/none/nothing
//      100 -> integer (32-bit)
//      101 -> string (packed SSO)
//      110 -> string (non-packed SSO)
//      111 -> <free>
//=====================================================================
inline static constexpr std::uint64_t fixed_type_mask = lshift(0x7, 48);
inline static constexpr std::uint64_t signature_mask = lshift(0xFFFF, 48);

inline static constexpr std::uint64_t fixed_type_nan_mask = lshift(0x0, 48);
inline static constexpr std::uint64_t fixed_type_false_mask = lshift(0x1, 48);
inline static constexpr std::uint64_t fixed_type_true_mask = lshift(0x2, 48);
inline static constexpr std::uint64_t fixed_type_null_mask = lshift(0x3, 48);

inline static constexpr std::uint64_t fixed_type_int32_mask = lshift(0x4, 48);
inline static constexpr std::uint64_t fixed_type_int32_data_mask = 0xFFFFFFFF;

//=====================================================================
// NaN box small string (N == 6) optimization
//
//              +- Type ID = 101 = Packed String
//              |
//              ---
// 0[Exponent-]1101[Char 6][Char 5][Char 4][Char 3][Char 2][Char 1]
//=====================================================================
inline static constexpr std::uint64_t fixed_type_packed_string_mask = lshift(0x5, 48);

//=====================================================================
// NaN box small string (N < 6) optimization
//
//              +- Type ID = 110 = Non-Packed String
//              |
//              ---
// 0[Exponent-]1110[Length][Char 5][Char 4][Char 3][Char 2][Char 1]
//=====================================================================
inline static constexpr std::uint64_t fixed_type_nonpacked_string_mask = lshift(0x6, 48);

//=====================================================================
// NaN box small string optimization data mask (48 bits)
inline static constexpr std::uint64_t fixed_type_sso_string_data_mask = 0xFFFFFFFFFFFF;

//=====================================================================
// NaN box heap allocated type definition
//
// +- If set, signals an encoded pointer
// v
// 1[Exponent ]1---PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
//                 ^
//                 +- Encoded pointer value (lower 48 bits)
//
// Currently x86_64/aarch64/riscv64 implementations by default only use up to 48-bits of
// virtual address space, as that gives 256 TiB of memory, which is more than enough for today's
// current compute needs. Hopefuly if/when this breaks and more address space is
// needed, it will only be on 128-bit architecture so we can still do NaN-boxing :D
//=====================================================================
// References
// - https://standards.ieee.org/ieee/754/6210/
// - https://leonardschuetz.ch/blog/nan-boxing/
// - https://en.wikipedia.org/wiki/X86-64#Canonical_form_addresses
//=====================================================================

//=====================================================================
// Nan box fixed type value masks
//=====================================================================
inline static constexpr std::uint64_t fixed_type_qnan_value =
     (nan_exponent_mask | nan_quiet_mask | fixed_type_nan_mask) & ~sign_mask;

inline static constexpr std::uint64_t fixed_type_snan_value =
    (nan_exponent_mask | nan_quiet_mask | fixed_type_nan_mask) & ~sign_mask;

inline static constexpr std::uint64_t fixed_type_false_value =
    (nan_exponent_mask | nan_quiet_mask | fixed_type_false_mask) & ~sign_mask;

inline static constexpr std::uint64_t fixed_type_true_value =
    (nan_exponent_mask | nan_quiet_mask | fixed_type_true_mask) & ~sign_mask;

inline static constexpr std::uint64_t fixed_type_null_value =
    (nan_exponent_mask | nan_quiet_mask | fixed_type_null_mask) & ~sign_mask;

inline static constexpr std::uint64_t fixed_type_int32_value =
    (nan_exponent_mask | nan_quiet_mask | fixed_type_int32_mask) & ~sign_mask;

inline static constexpr std::uint64_t fixed_type_packed_string_value =
    (nan_exponent_mask | nan_quiet_mask |
    fixed_type_packed_string_mask) & ~sign_mask;

inline static constexpr std::uint64_t fixed_type_nonpacked_string_value =
    (nan_exponent_mask | nan_quiet_mask |
    fixed_type_nonpacked_string_mask) & ~sign_mask;

//=====================================================================
// Nan box heap type value masks
//=====================================================================
inline static constexpr std::uint64_t heap_type_value =
    sign_mask | nan_exponent_mask | nan_quiet_mask;

inline static constexpr std::uint64_t heap_type_data_mask = 0xFFFFFFFFFFFF;
// clang-format on

};

}  // namespace anb::detail
