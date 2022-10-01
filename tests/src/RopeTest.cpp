#include "Rope.hpp"

#include <gtest/gtest.h>

TEST(RopeTest, It_Appends_Correctly)
{
    Rope r;
    r.append("Test");

    ASSERT_EQ("Test", r.to_string());
    r.append("ing");
    ASSERT_EQ("Testing", r.to_string());
}

TEST(RopeTest, It_Prepends_Correctly)
{
    Rope r;
    r.prepend("ing");

    ASSERT_EQ("ing", r.to_string());

    r.prepend("Test");
    ASSERT_EQ("Testing", r.to_string());
}

TEST(RopeTest, It_Inserts_Correctly)
{
    Rope r;
    ASSERT_TRUE(r.insert(0, "Tt"));
    ASSERT_EQ("Tt", r.to_string());

    ASSERT_TRUE(r.insert(1, "e"));
    ASSERT_EQ("Tet", r.to_string());

    ASSERT_TRUE(r.insert(2, "s"));
    ASSERT_EQ("Test", r.to_string());
}

TEST(RopeTest, It_Ignores_Empty_Appends)
{
    Rope r;
    r.append("");
    ASSERT_EQ(0, r.size());
    ASSERT_EQ("", r.to_string());
}

TEST(RopeTest, It_Ignores_Empty_Prepends)
{
    Rope r;
    r.prepend("");
    ASSERT_EQ(0, r.size());
    ASSERT_EQ("", r.to_string());
}

TEST(RopeTest, It_Ignores_Empty_Inserts)
{
    Rope r;
    r.insert(0, "");
    ASSERT_EQ(0, r.size());
    ASSERT_EQ("", r.to_string());
}

TEST(RopeTest, It_Do_Not_Insert_In_Invalid_Position)
{
    Rope r;
    ASSERT_TRUE(r.insert(0, "Test"));
    ASSERT_FALSE(r.insert(10, "Test"));
}

TEST(RopeTest, It_Shows_The_Correct_Size)
{
    Rope r;

    ASSERT_EQ(0, r.size());
    r.append("1234");
    ASSERT_EQ(4, r.size());
}

TEST(RopeTest, It_Splits_Correctly)
{
    Rope r;
    r.append("12345");
    ASSERT_EQ("12", r.substring(0, 2));
    ASSERT_EQ("345", r.substring(2, 3));
}

TEST(RopeTest, It_Splits_Correctly_When_Size_Is_Bigger_Than_String)
{
    Rope r;
    r.append("12345");
    ASSERT_EQ("12345", r.substring(0, 100));
}

TEST(RopeTest, It_Splits_Correctly_When_Offset_Is_Bigger_Than_String)
{
    Rope r;
    r.append("12345");
    ASSERT_EQ("", r.substring(30, 2));
}

TEST(RopeTest, It_Erases_Correctly)
{
    Rope r;
    r.append("12345");
    ASSERT_TRUE(r.erase(0, 2));
    ASSERT_EQ("345", r.to_string());
}

TEST(RopeTest, It_Gets_Correct_Character)
{
    Rope r;
    r.append("12345");

#ifdef W5N_ROPE_UTF8_SUPPORT
    ASSERT_EQ("1", r.at(0));
    ASSERT_EQ("2", r.at(1));
    ASSERT_EQ("3", r.at(2));
    ASSERT_EQ("4", r.at(3));
    ASSERT_EQ("5", r.at(4));
    ASSERT_EQ("", r.at(5));
#else
    ASSERT_EQ('1', r.at(0));
    ASSERT_EQ('2', r.at(1));
    ASSERT_EQ('3', r.at(2));
    ASSERT_EQ('4', r.at(3));
    ASSERT_EQ('5', r.at(4));
    ASSERT_EQ('\0', r.at(5));
#endif
}

#ifdef W5N_ROPE_UTF8_SUPPORT
TEST(Utf8RopeTest, It_Erases_Correctly)
{
    Rope r;
    r.append("😀🙏😂🙎😄");
    ASSERT_TRUE(r.erase(1, 2));
    ASSERT_EQ("😀🙎😄", r.to_string());
}

TEST(Utf8RopeTest, It_Appends_Correctly)
{
    Rope r;
    r.append("😸😹😺");
    r.append("😀😁😂😃😄");

    ASSERT_EQ("😸😹😺😀😁😂😃😄", r.to_string());
}

TEST(Utf8RopeTest, It_Prepends_Correctly)
{
    Rope r;
    r.prepend("😀😁😂😃😄");

    ASSERT_EQ("😀😁😂😃😄", r.to_string());

    r.prepend("😸😹😺");
    ASSERT_EQ("😸😹😺😀😁😂😃😄", r.to_string());
}

TEST(Utf8RopeTest, It_Inserts_Correctly)
{
    Rope r;
    ASSERT_TRUE(r.insert(0, "😀"));
    ASSERT_EQ("😀", r.to_string());

    ASSERT_TRUE(r.insert(1, "😻"));
    ASSERT_EQ("😀😻", r.to_string());

    ASSERT_TRUE(r.insert(2, "🙏"));
    ASSERT_EQ("😀😻🙏", r.to_string());

    ASSERT_TRUE(r.insert(2, "🙍"));
    ASSERT_EQ("😀😻🙍🙏", r.to_string());
}

TEST(Utf8RopeTest, It_Do_Not_Insert_In_Invalid_Position)
{
    Rope r;
    ASSERT_TRUE(r.insert(0, "😀"));
    ASSERT_FALSE(r.insert(10, "😀"));
}

TEST(Utf8RopeTest, It_Shows_The_Correct_Size)
{
    Rope r;
    ASSERT_EQ(0, r.size());

    r.append("😀🙏🙍😻");
    ASSERT_EQ(16, r.size());
}

TEST(Utf8RopeTest, It_Shows_The_Correct_Char_Count)
{
    Rope r;

    ASSERT_EQ(0, r.size());
    r.append("😀🙏🙍😻");
    ASSERT_EQ(4, r.charCount());
}

TEST(Utf8RopeTest, It_Splits_Correctly)
{
    Rope r;
    r.append("😀🙏🙍😻");
    ASSERT_EQ("😀🙏", r.substring(0, 2));
    ASSERT_EQ("🙍😻", r.substring(2, 3));
}

TEST(Utf8RopeTest, It_Splits_Correctly_When_Size_Is_Bigger_Than_String)
{
    Rope r;
    r.append("😀🙏🙍😻");
    ASSERT_EQ("😀🙏🙍😻", r.substring(0, 100));
}

TEST(Utf8RopeTest, It_Splits_Correctly_When_Offset_Is_Bigger_Than_String)
{
    Rope r;
    r.append("😀🙏🙍😻");
    ASSERT_EQ("", r.substring(30, 2));
}
#endif
