#include <gtest/gtest.h>

#include <anb/object.hpp>

#include "test_allocator.hpp"

#include <string>
#include <vector>

TEST(anb, object_assign_to_string) {
  ma assign_alloc;

  auto validate_heap = [&assign_alloc](anb::object<ma> obj) {
    static const std::string lorem =
        "    Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
        "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim "
        "ad "
        "minim veniam, quis nostrud exercitation ullamco laboris nisi ut "
        "aliquip "
        "ex ea commodo consequat. Duis aute irure dolor in reprehenderit in "
        "voluptate velit esse cillum dolore eu fugiat nulla pariatur. "
        "Excepteur "
        "sint occaecat cupidatat non proident, sunt in culpa qui officia "
        "deserunt mollit anim id est laborum.";

    auto heap_str = obj.assign(assign_alloc, lorem);
    EXPECT_TRUE(heap_str.is_heap_string(assign_alloc));
    EXPECT_EQ(lorem, heap_str.as_string_heap(assign_alloc).view());
  };

  auto validate_packed = [&assign_alloc](anb::object<ma> obj) {
    static const std::string packed = "jon316";

    auto packed_str = obj.assign(assign_alloc, packed);
    EXPECT_TRUE(packed_str.is_sso_string());
    EXPECT_EQ(packed, packed_str.as_string_sso());
  };

  auto validate_nonpacked = [&assign_alloc](anb::object<ma> obj) {
    static const std::string nonpacked = "yoyo";
    auto nonpacked_str = obj.assign(assign_alloc, nonpacked);
    EXPECT_TRUE(nonpacked_str.is_sso_string());
    EXPECT_EQ(nonpacked, nonpacked_str.as_string_sso());
  };

  EXPECT_EQ(0, assign_alloc.allocated_objects_.size());

  {
    anb::object<ma> bool_f(false);
    validate_heap(bool_f);
    EXPECT_EQ(1, assign_alloc.allocated_objects_.size());

    validate_packed(bool_f);
    validate_nonpacked(bool_f);
  }
  {
    anb::object<ma> bool_t(true);
    validate_heap(bool_t);
    EXPECT_EQ(2, assign_alloc.allocated_objects_.size());

    validate_packed(bool_t);
    validate_nonpacked(bool_t);
  }

  auto make_dict = [&assign_alloc]() {
    auto large_dict = anb::object<ma>::make_dictionary(assign_alloc);
    auto large_dict_key = anb::object<ma>::make_string_heap(assign_alloc);
    large_dict_key.as_string_heap(assign_alloc).set("FAVOURITE_MOVIE");
    anb::object<ma> large_dict_val(static_cast<std::int32_t>(0xCAFEBAAD));
    anb::dictionary<ma>& ld = large_dict.as_dictionary(assign_alloc);
    ld.set<std::pair>({large_dict_key, large_dict_val});
    return large_dict;
  };
  {
    validate_heap(make_dict());
    EXPECT_EQ(4, assign_alloc.allocated_objects_.size());

    validate_packed(make_dict());
    validate_nonpacked(make_dict());
  }
  {
    anb::object<ma> fp64(123.456);
    validate_heap(fp64);
    EXPECT_EQ(7, assign_alloc.allocated_objects_.size());

    validate_packed(fp64);
    validate_nonpacked(fp64);
    EXPECT_EQ(7, assign_alloc.allocated_objects_.size());
  }
  {
    anb::object<ma> i32(static_cast<std::int32_t>(0xDEADBEEF));
    validate_heap(i32);
    EXPECT_EQ(8, assign_alloc.allocated_objects_.size());

    validate_packed(i32);
    validate_nonpacked(i32);
    EXPECT_EQ(8, assign_alloc.allocated_objects_.size());
  }

  auto make_list = [&assign_alloc]() {
    const auto qnan = anb::object<ma>::make_qnan();
    anb::object<ma> bool_f(false);
    const auto nothing = anb::object<ma>::make_nothing();
    anb::object<ma> i32(static_cast<std::int32_t>(0xCAFEBAAD));
    anb::object<ma> fp64(123.456);
    anb::object<ma> str_sso(std::string{"yo"});

    auto str_heap = anb::object<ma>::make_string_heap(assign_alloc);
    str_heap.as_string_heap(assign_alloc).set("It's a Wonderful Life :D");

    auto list = anb::object<ma>::make_list(assign_alloc);
    anb::list<ma>& l = list.as_list(assign_alloc);
    l.set(qnan, bool_f, nothing, i32, fp64, str_sso, str_heap);

    return list;
  };

  {
    validate_heap(make_list());
    EXPECT_EQ(10, assign_alloc.allocated_objects_.size());

    validate_packed(make_list());
    validate_nonpacked(make_list());
  }
  {
    const auto nothing = anb::object<ma>::make_nothing();
    validate_heap(nothing);
    EXPECT_EQ(13, assign_alloc.allocated_objects_.size());

    validate_packed(nothing);
    validate_nonpacked(nothing);
    EXPECT_EQ(13, assign_alloc.allocated_objects_.size());
  }
  {
    const auto qnan = anb::object<ma>::make_qnan();
    validate_heap(qnan);
    EXPECT_EQ(14, assign_alloc.allocated_objects_.size());

    validate_packed(qnan);
    validate_nonpacked(qnan);
    EXPECT_EQ(14, assign_alloc.allocated_objects_.size());
  }
  {
    anb::object<ma> sso("hello!");
    validate_heap(sso);
    EXPECT_EQ(15, assign_alloc.allocated_objects_.size());

    validate_packed(sso);
    validate_nonpacked(sso);
    EXPECT_EQ(15, assign_alloc.allocated_objects_.size());
  }
  {
    anb::object<ma> sso("oi");
    validate_heap(sso);
    EXPECT_EQ(16, assign_alloc.allocated_objects_.size());

    validate_packed(sso);
    validate_nonpacked(sso);
    EXPECT_EQ(16, assign_alloc.allocated_objects_.size());
  }

  auto make_str = [&assign_alloc]() {
    auto heap_str = anb::object<ma>::make_string_heap(assign_alloc);

    anb::string<ma>& t = heap_str.as_string_heap(assign_alloc);
    t.set("a very long string");
    return heap_str;
  };
  {
    validate_heap(make_str());
    EXPECT_EQ(17, assign_alloc.allocated_objects_.size());

    validate_packed(make_str());
    validate_nonpacked(make_str());
  }
  EXPECT_EQ(17, assign_alloc.allocated_objects_.size());
  while (!assign_alloc.allocated_objects_.empty()) {
    assign_alloc.pop();
  }
  EXPECT_EQ(0, assign_alloc.allocated_objects_.size());
}
