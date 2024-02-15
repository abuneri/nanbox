#include <gtest/gtest.h>

#include <anb/object.hpp>

#include "test_allocator.hpp"

TEST(anb, object_dictionary) {
  auto str_key = anb::object<ma>::make_string_heap(allocator);
  str_key.as_string_heap(allocator).set("FAVOURITE_MOVIE");

  auto str_val = anb::object<ma>::make_string_heap(allocator);
  str_val.as_string_heap(allocator).set("It's a Wonderful Life :D");

  auto dict = anb::object<ma>::make_dictionary(allocator);

  EXPECT_TRUE(dict.is_dictionary(allocator));
  anb::dictionary<ma>& d = dict.as_dictionary(allocator);
  d.set<std::pair>({str_key, str_val});
  EXPECT_EQ(sizeof(double), sizeof(dict));

  EXPECT_EQ(1, d.object_dict_.size());
  EXPECT_EQ("It's a Wonderful Life :D",
            d.object_dict_.at(str_key).as_string_heap(allocator).view());
  EXPECT_EQ(dict.hash(), dict.hash());

  auto large_dict = anb::object<ma>::make_dictionary(allocator);
  auto large_dict_key = anb::object<ma>::make_string_heap(allocator);
  large_dict_key.as_string_heap(allocator).set("FAVOURITE_MOVIE");
  anb::object<ma> large_dict_val(static_cast<std::int32_t>(0xCAFEBAAD));
  anb::dictionary<ma>& ld = large_dict.as_dictionary(allocator);
  ld.set<std::pair>({large_dict_key, large_dict_val});
  EXPECT_EQ(sizeof(double), sizeof(large_dict));

  EXPECT_NE(dict.hash(), large_dict.hash());

  anb::object<ma>::dealloc_heap<anb::dictionary>(dict, allocator);
  anb::object<ma>::dealloc_heap<anb::dictionary>(large_dict, allocator);
}
