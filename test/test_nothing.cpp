#include <gtest/gtest.h>

#include <anb/object.hpp>

#include "test_allocator.hpp"

TEST(anb, object_nothing) {
  const auto nothing = anb::object<ma>::make_nothing();
  EXPECT_EQ(sizeof(double), sizeof(nothing));

  EXPECT_TRUE(nothing.is_nothing());

  EXPECT_FALSE(anb::object<ma>(false).is_nothing());
  EXPECT_FALSE(anb::object<ma>::make_qnan().is_nothing());
  EXPECT_FALSE(anb::object<ma>(42).is_nothing());
  EXPECT_FALSE(anb::object<ma>(100.24).is_nothing());
  EXPECT_FALSE(anb::object<ma>(std::string{"yo"}).is_nothing());

  EXPECT_EQ(anb::object<ma>::make_nothing().hash(),
            anb::object<ma>::make_nothing().hash());
}
