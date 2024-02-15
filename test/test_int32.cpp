#include <gtest/gtest.h>

#include <anb/object.hpp>

#include "test_allocator.hpp"

TEST(anb, object_int32) {
  anb::object<ma> i32(static_cast<std::int32_t>(0xDEADBEEF));
  EXPECT_EQ(sizeof(double), sizeof(i32));

  EXPECT_TRUE(i32.is_int32());
  EXPECT_EQ(0xDEADBEEF, i32.as_int32());

  EXPECT_FALSE(anb::object<ma>(false).is_int32());
  EXPECT_FALSE(anb::object<ma>::make_nothing().is_int32());
  EXPECT_FALSE(anb::object<ma>::make_qnan().is_int32());
  EXPECT_FALSE(anb::object<ma>(100.24).is_int32());

  EXPECT_EQ(anb::object<ma>{std::int32_t{42}}.hash(),
            anb::object<ma>{std::int32_t{42}}.hash());
  EXPECT_NE(anb::object<ma>{static_cast<std::int32_t>(0xDEADBEEF)}.hash(),
            anb::object<ma>{static_cast<std::int32_t>(0xCAFEBAAD)}.hash());
}
