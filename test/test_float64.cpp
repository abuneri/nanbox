#include <gtest/gtest.h>

#include <anb/object.hpp>

#include "test_allocator.hpp"

TEST(anb, object_float64) {
  anb::object<ma> fp64(123.456);
  EXPECT_EQ(sizeof(double), sizeof(fp64));

  EXPECT_TRUE(fp64.is_float64());
  EXPECT_EQ(123.456, fp64.as_float64());

  EXPECT_FALSE(anb::object<ma>(false).is_float64());
  EXPECT_FALSE(anb::object<ma>::make_nothing().is_float64());
  EXPECT_FALSE(anb::object<ma>::make_qnan().is_float64());
  EXPECT_FALSE(anb::object<ma>(42).is_float64());
  EXPECT_FALSE(anb::object<ma>(std::string{"yo"}).is_float64());

  EXPECT_EQ(anb::object<ma>{double{100.24}}.hash(),
            anb::object<ma>{double{100.24}}.hash());
  EXPECT_NE(anb::object<ma>{double{54.321}}.hash(),
            anb::object<ma>{double{12.345}}.hash());
}
