#include <gtest/gtest.h>

#include <anb/object.hpp>

#include "test_allocator.hpp"

auto validate_string_roundtrip_heap = [](ma& allocator,
                                         const std::string_view& s) {
  auto heap_str = anb::object<ma>::make_string_heap(allocator);
  EXPECT_EQ(sizeof(double), sizeof(heap_str));

  EXPECT_TRUE(heap_str.is_heap_string(allocator));
  anb::string<ma>& t = heap_str.as_string_heap(allocator);
  t.set(s);
  EXPECT_EQ(s, t.view());

  anb::object<ma>::dealloc_heap<anb::string>(heap_str, allocator);
};

TEST(anb, object_string_heap) {
  const std::string s = "Hello, World!";
  validate_string_roundtrip_heap(allocator, s);

  const std::string lorem =
      "    Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
      "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad "
      "minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip "
      "ex ea commodo consequat. Duis aute irure dolor in reprehenderit in "
      "voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur "
      "sint occaecat cupidatat non proident, sunt in culpa qui officia "
      "deserunt mollit anim id est laborum.";
  validate_string_roundtrip_heap(allocator, lorem);

  EXPECT_FALSE(anb::object<ma>::make_qnan().is_heap_string(allocator));
  EXPECT_FALSE(anb::object<ma>(false).is_heap_string(allocator));
  EXPECT_FALSE(anb::object<ma>::make_nothing().is_heap_string(allocator));
  EXPECT_FALSE(anb::object<ma>(42).is_heap_string(allocator));
  EXPECT_FALSE(anb::object<ma>(100.24).is_heap_string(allocator));

  auto lorem_str = anb::object<ma>::make_string_heap(allocator);
  anb::string<ma>& ls = lorem_str.as_string_heap(allocator);
  ls.set(lorem);
  EXPECT_EQ(sizeof(double), sizeof(lorem_str));

  EXPECT_EQ(lorem_str.hash(), lorem_str.hash());

  auto hw_str = anb::object<ma>::make_string_heap(allocator);
  anb::string<ma>& hs = hw_str.as_string_heap(allocator);
  hs.set("Hello, World!");
  EXPECT_EQ(sizeof(double), sizeof(hw_str));

  EXPECT_NE(lorem_str.hash(), hw_str.hash());
}
