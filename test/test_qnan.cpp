#include <gtest/gtest.h>
#include <cmath>

#include <anb/object.hpp>

#include "test_allocator.hpp"

TEST(anb, object_qnan) {
  const auto qnan = anb::object<ma>::make_qnan();
  EXPECT_EQ(sizeof(double), sizeof(qnan));

  EXPECT_TRUE(qnan.is_qnan());
  EXPECT_TRUE(std::isnan(qnan.as_qnan()));

  EXPECT_FALSE(anb::object<ma>(false).is_qnan());
  EXPECT_FALSE(anb::object<ma>::make_nothing().is_qnan());
  EXPECT_FALSE(anb::object<ma>(42).is_qnan());
  EXPECT_FALSE(anb::object<ma>(100.24).is_qnan());
  EXPECT_FALSE(anb::object<ma>(std::string{"yo"}).is_qnan());

  EXPECT_EQ(anb::object<ma>::make_qnan().hash(),
            anb::object<ma>::make_qnan().hash());
}
