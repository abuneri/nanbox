#include <gtest/gtest.h>

#include <anb/object.hpp>

#include "test_allocator.hpp"

TEST(anb, object_list) {
  const auto qnan = anb::object<ma>::make_qnan();
  anb::object<ma> bool_f(false);
  const auto nothing = anb::object<ma>::make_nothing();
  anb::object<ma> i32(static_cast<std::int32_t>(0xCAFEBAAD));
  anb::object<ma> fp64(123.456);
  anb::object<ma> str_sso(std::string{"yo"});

  auto str_heap = anb::object<ma>::make_string_heap(allocator);
  str_heap.as_string_heap(allocator).set("It's a Wonderful Life :D");

  auto list = anb::object<ma>::make_list(allocator);

  EXPECT_TRUE(list.is_list(allocator));
  anb::list<ma>& l = list.as_list(allocator);
  l.set(qnan, bool_f, nothing, i32, fp64, str_sso, str_heap);
  EXPECT_EQ(sizeof(double), sizeof(list));

  EXPECT_EQ(7, l.objects_.size());

  EXPECT_TRUE(l.objects_.at(0).is_qnan());
  EXPECT_FALSE(l.objects_.at(1).as_boolean());
  EXPECT_TRUE(l.objects_.at(2).is_nothing());
  EXPECT_EQ(static_cast<std::int32_t>(0xCAFEBAAD), l.objects_.at(3).as_int32());
  EXPECT_EQ(123.456, l.objects_.at(4).as_float64());
  EXPECT_EQ("yo", l.objects_.at(5).as_string_sso());
  EXPECT_EQ("It's a Wonderful Life :D",
            l.objects_.at(6).as_string_heap(allocator).view());
  EXPECT_EQ(list.hash(), list.hash());

  auto smaller_list = anb::object<ma>::make_list(allocator);
  anb::list<ma>& sl = smaller_list.as_list(allocator);
  sl.set(qnan, bool_f, i32, fp64, str_heap);
  EXPECT_EQ(sizeof(double), sizeof(smaller_list));

  EXPECT_NE(list.hash(), smaller_list.hash());

  list.dealloc_heap(allocator);
  smaller_list.dealloc_heap(allocator);
}
