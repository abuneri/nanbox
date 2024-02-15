#include <gtest/gtest.h>

#include <anb/object.hpp>

#include "test_allocator.hpp"

TEST(anb, object_boolean) {
  anb::object<ma> bool_f(false);
  EXPECT_EQ(sizeof(double), sizeof(bool_f));

  EXPECT_TRUE(bool_f.is_boolean());
  EXPECT_FALSE(bool_f.as_boolean());

  anb::object<ma> bool_t(true);
  EXPECT_EQ(sizeof(double), sizeof(bool_t));

  EXPECT_TRUE(bool_t.is_boolean());
  EXPECT_TRUE(bool_t.as_boolean());

  EXPECT_FALSE(anb::object<ma>::make_qnan().is_boolean());
  EXPECT_FALSE(anb::object<ma>::make_nothing().is_boolean());
  EXPECT_FALSE(anb::object<ma>(42).is_boolean());
  EXPECT_FALSE(anb::object<ma>(100.24).is_boolean());
  EXPECT_FALSE(anb::object<ma>(std::string{"yo"}).is_boolean());

  EXPECT_EQ(anb::object<ma>{true}.hash(), anb::object<ma>{true}.hash());
  EXPECT_EQ(anb::object<ma>{false}.hash(), anb::object<ma>{false}.hash());
  EXPECT_NE(anb::object<ma>{true}.hash(), anb::object<ma>{false}.hash());
  EXPECT_NE(anb::object<ma>{false}.hash(), anb::object<ma>{true}.hash());
}
