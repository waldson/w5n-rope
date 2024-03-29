#include "w5n/Rope.hpp"

#include <gtest/gtest.h>

TEST(RopeTest, It_Appends_Correctly)
{
    w5n::Rope r;
    r.append("Test");

    ASSERT_EQ("Test", r.toString());
    r.append("ing");
    ASSERT_EQ("Testing", r.toString());
}

TEST(RopeTest, It_Prepends_Correctly)
{
    w5n::Rope r;
    r.prepend("ing");

    ASSERT_EQ("ing", r.toString());

    r.prepend("Test");
    ASSERT_EQ("Testing", r.toString());
}

TEST(RopeTest, It_Inserts_Correctly)
{
    w5n::Rope r;
    ASSERT_TRUE(r.insert(0, "Tt"));
    ASSERT_EQ("Tt", r.toString());

    ASSERT_TRUE(r.insert(1, "e"));
    ASSERT_EQ("Tet", r.toString());

    ASSERT_TRUE(r.insert(2, "s"));
    ASSERT_EQ("Test", r.toString());
}

TEST(RopeTest, It_Ignores_Empty_Appends)
{
    w5n::Rope r;
    r.append("");
    ASSERT_EQ(0, r.size());
    ASSERT_EQ("", r.toString());
}

TEST(RopeTest, It_Ignores_Empty_Prepends)
{
    w5n::Rope r;
    r.prepend("");
    ASSERT_EQ(0, r.size());
    ASSERT_EQ("", r.toString());
}

TEST(RopeTest, It_Ignores_Empty_Inserts)
{
    w5n::Rope r;
    r.insert(0, "");
    ASSERT_EQ(0, r.size());
    ASSERT_EQ("", r.toString());
}

TEST(RopeTest, It_Do_Not_Insert_In_Invalid_Position)
{
    w5n::Rope r;
    ASSERT_TRUE(r.insert(0, "Test"));
    ASSERT_FALSE(r.insert(10, "Test"));
}

TEST(RopeTest, It_Shows_The_Correct_Size)
{
    w5n::Rope r;

    ASSERT_EQ(0, r.size());
    r.append("1234");
    ASSERT_EQ(4, r.size());
}

TEST(RopeTest, It_Splits_Correctly)
{
    w5n::Rope r;
    r.append("12345");
    ASSERT_EQ("12", r.substring(0, 2));
    ASSERT_EQ("345", r.substring(2, 3));
}

TEST(RopeTest, It_Splits_Correctly_When_Size_Is_Bigger_Than_String)
{
    w5n::Rope r;
    r.append("12345");
    ASSERT_EQ("12345", r.substring(0, 100));
}

TEST(RopeTest, It_Splits_Correctly_When_Offset_Is_Bigger_Than_String)
{
    w5n::Rope r;
    r.append("12345");
    ASSERT_EQ("", r.substring(30, 2));
}

TEST(RopeTest, It_Erases_Correctly)
{
    w5n::Rope r;
    r.append("12345");
    ASSERT_TRUE(r.erase(0, 2));
    ASSERT_EQ("345", r.toString());
}

TEST(RopeTest, It_Gets_Correct_Character)
{
    w5n::Rope r;

#ifndef W5N_ROPE_UTF8_IGNORE
    r.append("😀😁😂😃😄😅👶🏽ç");
    ASSERT_EQ("😀", r.at(0));
    ASSERT_EQ("😁", r.at(1));
    ASSERT_EQ("😂", r.at(2));
    ASSERT_EQ("😃", r.at(3));
    ASSERT_EQ("😄", r.at(4));
    ASSERT_EQ("😅", r.at(5));
    ASSERT_EQ("👶🏽", r.at(6)); // baby has 2 code points
    ASSERT_EQ("ç", r.at(7));
    ASSERT_EQ("", r.at(8));
#else
    r.append("12345");
    ASSERT_EQ('1', r.at(0));
    ASSERT_EQ('2', r.at(1));
    ASSERT_EQ('3', r.at(2));
    ASSERT_EQ('4', r.at(3));
    ASSERT_EQ('5', r.at(4));
    ASSERT_EQ('\0', r.at(5));
#endif
}

#ifndef W5N_ROPE_UTF8_IGNORE
TEST(Utf8RopeTest, It_Erases_Correctly)
{
    w5n::Rope r;
    r.append("😀🙏😂🙎😄");
    ASSERT_TRUE(r.erase(1, 2));
    ASSERT_EQ("😀🙎😄", r.toString());
}

TEST(Utf8RopeTest, It_Erases_Correctly_With_Grapheme)
{
    w5n::Rope r;
    r.append("😀👶🏽😂🙎😄");
    ASSERT_TRUE(r.erase(1, 2));
    ASSERT_EQ("😀🙎😄", r.toString());
}

TEST(Utf8RopeTest, It_Appends_Correctly)
{
    w5n::Rope r;
    r.append("😸😹😺");
    ASSERT_EQ("😸😹😺", r.toString());

    r.append("😀😁😂😃😄");
    ASSERT_EQ("😸😹😺😀😁😂😃😄", r.toString());
}

TEST(Utf8RopeTest, It_Prepends_Correctly)
{
    w5n::Rope r;
    r.prepend("😀😁😂😃😄");

    ASSERT_EQ("😀😁😂😃😄", r.toString());

    r.prepend("😸😹😺");
    ASSERT_EQ("😸😹😺😀😁😂😃😄", r.toString());
}

TEST(Utf8RopeTest, It_Inserts_Correctly)
{
    w5n::Rope r;
    ASSERT_TRUE(r.insert(0, "😀"));
    ASSERT_EQ("😀", r.toString());

    ASSERT_TRUE(r.insert(1, "😻"));
    ASSERT_EQ("😀😻", r.toString());

    ASSERT_TRUE(r.insert(2, "🙏"));
    ASSERT_EQ("😀😻🙏", r.toString());

    ASSERT_TRUE(r.insert(2, "🙍"));
    ASSERT_EQ("😀😻🙍🙏", r.toString());
}

TEST(Utf8RopeTest, It_Do_Not_Insert_In_Invalid_Position)
{
    w5n::Rope r;
    ASSERT_TRUE(r.insert(0, "😀"));
    ASSERT_FALSE(r.insert(10, "😀"));
}

TEST(Utf8RopeTest, It_Shows_The_Correct_Size)
{
    w5n::Rope r;
    ASSERT_EQ(0, r.size());

    r.append("😀🙏🙍😻👶🏽"); // 4 + 4 + 4 + 8 (the baby is composed of 2 code points)
    ASSERT_EQ(24, r.size());
}

TEST(Utf8RopeTest, It_Shows_The_Correct_Char_Count_With_Graphemes)
{
    w5n::Rope r;

    ASSERT_EQ(0, r.size());
    r.append("😀🙏🙍😻");
    ASSERT_EQ(4, r.charCount());
    r.append("👶🏽"); // this baby has 2 code points
    ASSERT_EQ(5, r.charCount());
}

TEST(Utf8RopeTest, It_Gets_The_Correct_Substring_With_Grapheme)
{
    w5n::Rope r;
    r.append("👶🏽😻🙍😀");
    ASSERT_EQ("👶🏽😻", r.substring(0, 2));
    ASSERT_EQ("🙍😀", r.substring(2, 2));
}

TEST(Utf8RopeTest, It_Splits_Correctly)
{
    w5n::Rope r;
    r.append("😀🙏🙍😻");
    ASSERT_EQ("😀🙏", r.substring(0, 2));
    ASSERT_EQ("🙍😻", r.substring(2, 3));
}

TEST(Utf8RopeTest, It_Splits_Correctly_When_Size_Is_Bigger_Than_String)
{
    w5n::Rope r;
    r.append("😀🙏🙍😻");
    ASSERT_EQ("😀🙏🙍😻", r.substring(0, 100));
}

TEST(Utf8RopeTest, It_Splits_Correctly_When_Offset_Is_Bigger_Than_String)
{
    w5n::Rope r;
    r.append("😀🙏🙍😻");
    ASSERT_EQ("", r.substring(30, 2));
}
#endif
