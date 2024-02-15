#include <gtest/gtest.h>

#include <anb/object.hpp>

#include "test_allocator.hpp"

auto validate_string_roundtrip_sso = [](const std::string_view& s) {
  anb::object<ma> str(s);
  EXPECT_EQ(sizeof(double), sizeof(str));

  EXPECT_TRUE(str.is_sso_string());
  EXPECT_EQ(s, str.as_string_sso());
};

TEST(anb, object_string_sso_packed) {
  const char cs_6[7] = {static_cast<char>(159),
                        static_cast<char>(55),
                        static_cast<char>(89),
                        static_cast<char>(169),
                        static_cast<char>(191),
                        static_cast<char>(64),
                        '\0'};
  validate_string_roundtrip_sso(std::string{cs_6});

  const std::string s = "hello!";
  anb::object<ma> str(s);
  EXPECT_EQ(sizeof(double), sizeof(str));

  EXPECT_TRUE(str.is_sso_string());
  EXPECT_EQ(s, str.as_string_sso());

  EXPECT_FALSE(anb::object<ma>::make_qnan().is_sso_string());
  EXPECT_FALSE(anb::object<ma>(false).is_sso_string());
  EXPECT_FALSE(anb::object<ma>::make_nothing().is_sso_string());
  EXPECT_FALSE(anb::object<ma>(42).is_sso_string());
  EXPECT_FALSE(anb::object<ma>(100.24).is_sso_string());

  EXPECT_EQ(anb::object<ma>{std::string{"jon316"}}.hash(),
            anb::object<ma>{std::string{"jon316"}}.hash());
  EXPECT_NE(anb::object<ma>{std::string{"yo"}}.hash(),
            anb::object<ma>{std::string{"oi"}}.hash());
}

TEST(anb, object_string_sso_nonpacked) {
  validate_string_roundtrip_sso(std::string{""});

  const char cs_1[2] = {static_cast<char>(159), '\0'};
  validate_string_roundtrip_sso(std::string{cs_1});
  validate_string_roundtrip_sso(std::string{"W"});

  const char cs_2[3] = {static_cast<char>(159), static_cast<char>(55), '\0'};
  validate_string_roundtrip_sso(std::string{cs_2});
  validate_string_roundtrip_sso(std::string{"yo"});

  const char cs_3[4] = {static_cast<char>(159), static_cast<char>(55),
                        static_cast<char>(89), '\0'};
  validate_string_roundtrip_sso(std::string{cs_3});
  validate_string_roundtrip_sso(std::string{"hel"});

  const char cs_4[5] = {static_cast<char>(159), static_cast<char>(55),
                        static_cast<char>(89), static_cast<char>(35), '\0'};
  validate_string_roundtrip_sso(std::string{cs_4});
  validate_string_roundtrip_sso(std::string{"hell"});

  const char cs_5[6] = {static_cast<char>(159), static_cast<char>(55),
                        static_cast<char>(89),  static_cast<char>(35),
                        static_cast<char>(71),  '\0'};
  validate_string_roundtrip_sso(std::string{cs_5});
  validate_string_roundtrip_sso(std::string{"hello"});

  EXPECT_FALSE(anb::object<ma>::make_qnan().is_sso_string());
  EXPECT_FALSE(anb::object<ma>(false).is_sso_string());
  EXPECT_FALSE(anb::object<ma>::make_nothing().is_sso_string());
  EXPECT_FALSE(anb::object<ma>(42).is_sso_string());
  EXPECT_FALSE(anb::object<ma>(100.24).is_sso_string());
}
