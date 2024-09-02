#include "stralgo.h"

#include "gtest/gtest.h"

using namespace std::string_view_literals;

namespace str = ini::str;

TEST(Trim, Default)
{
    EXPECT_EQ(str::trim("test"),       "test"sv);
    EXPECT_EQ(str::trim("te st"),      "te st"sv);
    EXPECT_EQ(str::trim("  test"),     "test"sv);
    EXPECT_EQ(str::trim("    "),       ""sv);
    EXPECT_EQ(str::trim("test  "),     "test"sv);
    EXPECT_EQ(str::trim("\t\rtest  "), "test"sv);
    EXPECT_EQ(str::trim("\tte  st  "), "te  st"sv);
}

TEST(ExtractSectionName, Default)
{
    EXPECT_EQ(str::extract_section_name("[section]"), "section"sv);
    EXPECT_EQ(str::extract_section_name("[test]"),    "test"sv);
}

TEST(ExtractSectionName, BlankChar)
{
    EXPECT_EQ(str::extract_section_name("  [test]  "),        "test"sv);
    EXPECT_EQ(str::extract_section_name("  [field field] "),  "field field"sv);
    EXPECT_EQ(str::extract_section_name("  [  test tes ] "),  "test tes"sv);
}

TEST(ExtractSectionName, InvalidInput)
{
    EXPECT_EQ(str::extract_section_name("[  "),     ""sv);
    EXPECT_EQ(str::extract_section_name(" []8  "),  ""sv);
    EXPECT_EQ(str::extract_section_name("  [] "),   ""sv);
    EXPECT_EQ(str::extract_section_name("  [  ] "), ""sv);
    EXPECT_EQ(str::extract_section_name("【】"),    ""sv);
    EXPECT_EQ(str::extract_section_name("!A[fha]"), ""sv);
    EXPECT_EQ(str::extract_section_name("]  ["),    ""sv);
}

TEST(ExtractKeyValue, Default)
{
    EXPECT_EQ(str::extract_key_value("test=2").key,               "test"sv);
    EXPECT_EQ(str::extract_key_value("test=2").value,             "2"sv);
    EXPECT_EQ(str::extract_key_value("A BcdEf=1949 10 01").key,   "A BcdEf"sv);
    EXPECT_EQ(str::extract_key_value("A BcdEf=1949 10 01").value, "1949 10 01"sv);
    EXPECT_EQ(str::extract_key_value("Biden=").key,               "Biden"sv);
    EXPECT_EQ(str::extract_key_value("Biden=").value,             ""sv);
}

TEST(ExtractKeyValue, BlankChar)
{
    EXPECT_EQ(str::extract_key_value("  test \t= 100\r ").key,   "test"sv);
    EXPECT_EQ(str::extract_key_value("  test \t= 100\r ").value, "100"sv);
    EXPECT_EQ(str::extract_key_value("  A B  =194910   ").key,   "A B"sv);
    EXPECT_EQ(str::extract_key_value("  A B  =194910   ").value, "194910"sv);
}

TEST(ExtractKeyValue, InvalidInput)
{
    EXPECT_EQ(str::extract_key_value("= 100\r ").key,   ""sv);
    EXPECT_EQ(str::extract_key_value("= 100\r ").value, "100"sv);
    EXPECT_EQ(str::extract_key_value("  ==").key,       ""sv);
    EXPECT_EQ(str::extract_key_value(  "==").value,     "="sv);
    EXPECT_EQ(str::extract_key_value("A Happy").key,    ""sv);
    EXPECT_EQ(str::extract_key_value("A Happy").value,  ""sv);
}

TEST(IsEmptyLine, Default)
{
    EXPECT_TRUE(str::is_empty_line(""));
    EXPECT_TRUE(str::is_empty_line("  "));
    EXPECT_TRUE(str::is_empty_line("\n\r"));
    EXPECT_TRUE(str::is_empty_line("\t\n"));
    EXPECT_FALSE(str::is_empty_line("   8"));
    EXPECT_FALSE(str::is_empty_line("--"));
}

TEST(EraseComments, LineComments)
{
    EXPECT_EQ(str::erase_comments("# 100008 ;"), ""sv);
    EXPECT_EQ(str::erase_comments("; 10000 #"), ""sv);
    EXPECT_EQ(str::erase_comments("  # 100& "), "  "sv);
    EXPECT_EQ(str::erase_comments("\t;1000\t"), "\t"sv);
}

TEST(EraseComments, InlineComments)
{
    EXPECT_EQ(str::erase_comments("key: value # 1000"), "key: value "sv);
    EXPECT_EQ(str::erase_comments("key: value ; 1000"), "key: value "sv);
    EXPECT_EQ(str::erase_comments("\tkey: value# 1000"), "\tkey: value"sv);
    EXPECT_EQ(str::erase_comments("  key: value; 1000"), "  key: value"sv);
}

TEST(EraseComments, NoComments)
{
    EXPECT_EQ(str::erase_comments("key: value"), "key: value"sv);
    EXPECT_EQ(str::erase_comments("I'm Happy "), "I'm Happy "sv);
    EXPECT_EQ(str::erase_comments("what's cmt"), "what's cmt"sv);
}
