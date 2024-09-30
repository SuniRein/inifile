#include "inifile/inifile.h"

#include "gtest/gtest.h"

#include <cstdint>
#include <string_view>

#define INI_UNUSED(expr) (void)(expr)

/// HelloWorld-like test.
TEST(DecodeBasic, HelloWorld)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Hello World]\nkey = value"));
    EXPECT_EQ(file.size(), 1);
    EXPECT_EQ(file["Hello World"].size(), 1);
    EXPECT_EQ(file["Hello World"]["key"].as_str(), "value");
}

/// Process multiple sections.
TEST(DecodeBasic, MultiSection)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section 1]\nkey1=value1\n[Section 2]\nkey2=value2"));
    EXPECT_EQ(file.size(), 2);
    EXPECT_EQ(file["Section 1"].size(), 1);
    EXPECT_EQ(file["Section 1"]["key1"].as_str(), "value1");
    EXPECT_EQ(file["Section 2"].size(), 1);
    EXPECT_EQ(file["Section 2"]["key2"].as_str(), "value2");
}

/// Process section with multiple keys.
TEST(DecodeBasic, MultiKey)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section]\nkey1=value1\nkey2=value2"));
    EXPECT_EQ(file.size(), 1);
    EXPECT_EQ(file["Section"].size(), 2);
    EXPECT_EQ(file["Section"]["key1"].as_str(), "value1");
    EXPECT_EQ(file["Section"]["key2"].as_str(), "value2");
}

/// Empty sections will be ignored.
/// TODO: Add config option for it.
TEST(DecodeEmpty, EmptySection)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section 1]\n[Section 2]\n[Section 3]"));
    EXPECT_EQ(file.size(), 0);
}

/// Key with empty value will be reserved.
/// TODO: Add config option for it.
TEST(DecodeEmpty, KeyWithEmptyValue)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section]\nkey="));
    EXPECT_EQ(file["Section"].size(), 1);
    EXPECT_EQ(file["Section"]["key"].as_str(), "");
}

/// Process blank chars around section signs `[]`.
/// The blank chars surround it will be removed.
/// TODO: Add support for it.
TEST(DecodeBlank, AroundSectionSign)
{
    ini::File file;
    ASSERT_TRUE(file.decode("\t  [Name]\t \nkey=value\n\b  [Hello World]   \nkey=value"));
    EXPECT_EQ(file.size(), 2);
    EXPECT_NE(file.find("Name"), file.end());
    EXPECT_NE(file.find("Hello World"), file.end());
}

/// Process blank chars in section name.
/// The blank chars surround a section name will be removed.
/// TODO: Add config option for it.
TEST(DecodeBlank, InSection)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[\tName ]\nkey=value\n[ \tHello World ]\nkey=value"));
    EXPECT_EQ(file.size(), 2);
    EXPECT_NE(file.find("Name"), file.end());
    EXPECT_NE(file.find("Hello World"), file.end());
}

/// Process blank chars in key-value pairs.
/// The blank chars surround a key or a value will be removed.
/// TODO: Add config option for it.
TEST(DecodeBlank, InKeyValue)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section]\n\tkey1 = value1\t\r\nke y2= va lue\t"));
    EXPECT_EQ(file["Section"]["key1"].as_str(), "value1");
    EXPECT_EQ(file["Section"]["ke y2"].as_str(), "va lue");
}

/// Porcess special char '='
TEST(DecodeSpecialChar, EqualSign)
{
    ini::File file;
    // TODO: Now the first '=' will be recognized as seperator between key and value.
    //       '\=' escape is not supported now.
    ASSERT_TRUE(file.decode("[Section]\n<\\=>=0\n<\\=\\=>=1\nright=1\\=2\\=3"));
    EXPECT_EQ(file["Section"]["<=>"].as_str(), "0");
    EXPECT_EQ(file["Section"]["<==>"].as_str(), "1");
    EXPECT_EQ(file["Section"]["right"].as_str(), "2=3");
}

/// Process special char '#'
TEST(DecodeSpecialChar, NumberSign)
{
    ini::File file;
    // TODO: Now '#' will be recognized as the start of a comment.
    //       '\#' escape is not supported now.
    ASSERT_TRUE(file.decode("[Section]\n\\#1=A\n\\#2=\\#B\n3=\\#BC"));
    EXPECT_EQ(file["Section"]["#1"].as_str(), "A");
    EXPECT_EQ(file["Section"]["#2"].as_str(), "#B");
    EXPECT_EQ(file["Section"]["3"].as_str(), "#BC");
}

/// Process special char ';'
TEST(DecodeSpecialChar, SemicolonSign)
{
    ini::File file;
    // TODO: Now ';' will be recognized as the start of a comment.
    //       '\;' escape is not supported now.
    ASSERT_TRUE(file.decode("[Section]\n\\;1=A\n\\;2=\\;B\n3=\\;BC"));
    EXPECT_EQ(file["Section"][";1"].as_str(), "A");
    EXPECT_EQ(file["Section"][";2"].as_str(), ";B");
    EXPECT_EQ(file["Section"]["3"].as_str(), ";BC");
}

/// Process common escape char.
TEST(DecodeSpecialChar, CommonEscape)
{
    std::string_view input_text =
R"([Section]
LN=\n
ET=\r
TB=\t
BS=\b
AL=\a
NL=\0
SQ=\'
DQ=\"
XF=\x001
)";

    ini::File file;
    // TODO: Escape char is not supported now.
    ASSERT_TRUE(file.decode(input_text));
    EXPECT_EQ(file["Section"]["LN"].as_str(), "\n");
    EXPECT_EQ(file["Section"]["ET"].as_str(), "\r");
    EXPECT_EQ(file["Section"]["TB"].as_str(), "\t");
    EXPECT_EQ(file["Section"]["BS"].as_str(), "\b");
    EXPECT_EQ(file["Section"]["AL"].as_str(), "\a");
    EXPECT_EQ(file["Section"]["NL"].as_str(), "\0");
    EXPECT_EQ(file["Section"]["SQ"].as_str(), "\'");
    EXPECT_EQ(file["Section"]["DQ"].as_str(), "\"");
    EXPECT_EQ(file["Section"]["XF"].as_str(), "\x001");
}

/// Line comments start with '#' or ';' will be ignored.
/// TODO: Add config option for comment signs.
TEST(DecodeComment, LineComment)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section]\n# This is a comment.\nkey = value\n; This is another comment."));
    EXPECT_EQ(file["Section"].size(), 1);
    EXPECT_EQ(file["Section"]["key"].as_str(), "value");
}

/// Inline comments start with '#' or ';' will be ignored.
/// TODO: Add config option for inline comments.
TEST(DecodeComment, InlineComment)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section] # This is a comment.\nkey = value ; This is another comment."));
    EXPECT_EQ(file["Section"].size(), 1);
    EXPECT_EQ(file["Section"]["key"].as_str(), "value");
}

/// Inline comments adjacent to valid content with be ignored.
TEST(DecodeComment, InlineCommentAdjacent)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section]#This is a comment.\nkey = value;This is another comment."));
    EXPECT_EQ(file["Section"].size(), 1);
    EXPECT_EQ(file["Section"]["key"].as_str(), "value");
}

/// Decode decimal integral type.
/// TODO: Add support for other base.
TEST(DecodeIntegral, CommonDecimal)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section]\na=3\nb=0\nc=-2"));
    EXPECT_EQ(file["Section"]["a"].to<int>(), 3);
    EXPECT_EQ(file["Section"]["b"].to<long long>(), 0LL);
    EXPECT_EQ(file["Section"]["c"].to<int>(), -2);
}

/// Decode integral type with out of range error.
TEST(DecodeIntegral, OutOfRange)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section]\na=-1\nb=1000000000000"));

    // negative number to unsigned
    EXPECT_THROW(INI_UNUSED(file["Section"]["a"].to<unsigned>()), ini::DecodeError);
    EXPECT_NO_THROW(INI_UNUSED(file["Section"]["a"].to<int>()));

    // number too large that out of range
    EXPECT_THROW(INI_UNUSED(file["Section"]["b"].to<int>()), ini::DecodeError);
    EXPECT_NO_THROW(INI_UNUSED(file["Section"]["b"].to<int64_t>()));
}

/// Decode integral type with invalid argument.
TEST(DecodeIntegral, InvalidArgument)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section]\na=#33\nb=true\nc=string"));
    EXPECT_THROW(INI_UNUSED(file["Section"]["a"].to<int>()), ini::DecodeError);
    EXPECT_THROW(INI_UNUSED(file["Section"]["b"].to<int>()), ini::DecodeError);
    EXPECT_THROW(INI_UNUSED(file["Section"]["c"].to<int>()), ini::DecodeError);
}

/// Decode integral type with floating point input.
/// TODO: Add config option for it.
TEST(DecodeIntegral, FloatingPoint)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section]\na=1.\nb=1.8\nc=-1.8"));
    EXPECT_EQ(file["Section"]["a"].to<int>(), 1);
    EXPECT_EQ(file["Section"]["b"].to<int>(), 1);
    EXPECT_EQ(file["Section"]["c"].to<int>(), -1);
}

/// Decode integral type with partial right.
/// TODO: Add config option for it.
TEST(DecodeIntegral, PartialRight)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section]\na=1#*\nb=-1&\nc=78)"));
    EXPECT_EQ(file["Section"]["a"].to<int>(), 1);
    EXPECT_EQ(file["Section"]["b"].to<int>(), -1);
    EXPECT_EQ(file["Section"]["c"].to<int>(), 78);
}

/// Decode integral type with informated input.
/// TODO: Add support for it.
TEST(DecodeIntegral, InformattedInput)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section]\na=123,456\nb=-123,456\nc=7,123,0000)"));
    EXPECT_EQ(file["Section"]["a"].to<int>(), 123);
    EXPECT_EQ(file["Section"]["b"].to<int>(), -123);
    EXPECT_EQ(file["Section"]["c"].to<int>(), 7);
}

/// Decode decimal floating point type.
/// TODO: Add support for other base.
TEST(DecodeFlotingPoint, CommonDecimal)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section]\na=1.06\nb=-1.2\nc=0)"));
    EXPECT_EQ(file["Section"]["a"].to<float>(), 1.06F);
    EXPECT_EQ(file["Section"]["b"].to<double>(), -1.2);
    EXPECT_EQ(file["Section"]["c"].to<float>(), 0.0F);
}

/// Decode floating point type with scientific format.
TEST(DecodeFlotingPoint, Scientific)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section]\na=1e6\nb=-1E2\nc=1e-1)"));
    EXPECT_EQ(file["Section"]["a"].to<float>(), 1e6F);
    EXPECT_EQ(file["Section"]["b"].to<double>(), -1E2);
    EXPECT_EQ(file["Section"]["c"].to<float>(), 1e-1F);
}

/// Decode floating point type with out of range error.
TEST(DecodeFlotingPoint, OutOfRange)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section]\na=-1e50\nb=1e310\nc=1e-46\nd=1e-326"));

    // large: float(x) double(/)
    EXPECT_THROW(INI_UNUSED(file["Section"]["a"].to<float>()), ini::DecodeError);
    EXPECT_NO_THROW(INI_UNUSED(file["Section"]["a"].to<double>()));

    // number too large
    EXPECT_THROW(INI_UNUSED(file["Section"]["b"].to<float>()), ini::DecodeError);
    EXPECT_THROW(INI_UNUSED(file["Section"]["b"].to<double>()), ini::DecodeError);

    // small: float(x) double(/)
    EXPECT_THROW(INI_UNUSED(file["Section"]["c"].to<float>()), ini::DecodeError);
    EXPECT_NO_THROW(INI_UNUSED(file["Section"]["c"].to<double>()));

    // number too small
    EXPECT_THROW(INI_UNUSED(file["Section"]["d"].to<float>()), ini::DecodeError);
    EXPECT_THROW(INI_UNUSED(file["Section"]["d"].to<double>()), ini::DecodeError);
}

/// Decode floating point type with invalid argument.
TEST(DecodeFlotingPoint, InvalidArgument)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section]\na=#33\nb=true\nc=string"));
    EXPECT_THROW(INI_UNUSED(file["Section"]["a"].to<float>()), ini::DecodeError);
    EXPECT_THROW(INI_UNUSED(file["Section"]["b"].to<double>()), ini::DecodeError);
    EXPECT_THROW(INI_UNUSED(file["Section"]["c"].to<long double>()), ini::DecodeError);
}

/// Decode floating point type with partial right.
/// TODO: Add config option for it.
TEST(DecodeFlotingPoint, PartialRight)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section]\na=1.0#*\nb=-1-&\nc=78.2())"));
    EXPECT_EQ(file["Section"]["a"].to<double>(), 1.0);
    EXPECT_EQ(file["Section"]["b"].to<double>(), -1.0);
    EXPECT_EQ(file["Section"]["c"].to<double>(), 78.2);
}

/// Decode boolean type.
/// Valid type: True, Flase, true, false
/// TODO: Add config option for valid form.
TEST(DocodeBoolean, Valid)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section]\na=True\nb=true\nc=False\nd=false"));
    EXPECT_EQ(file["Section"]["a"].to<bool>(), true);
    EXPECT_EQ(file["Section"]["b"].to<bool>(), true);
    EXPECT_EQ(file["Section"]["c"].to<bool>(), false);
    EXPECT_EQ(file["Section"]["d"].to<bool>(), false);
}

/// Decode invalid boolean type.
TEST(DocodeBoolean, Invalid)
{
    ini::File file;
    ASSERT_TRUE(file.decode("[Section]\na=TRUE\nb=1\nc=ture\nd=False+"));
    EXPECT_THROW(INI_UNUSED(file["Section"]["a"].to<bool>()), ini::DecodeError);
    EXPECT_THROW(INI_UNUSED(file["Section"]["b"].to<bool>()), ini::DecodeError);
    EXPECT_THROW(INI_UNUSED(file["Section"]["c"].to<bool>()), ini::DecodeError);
    EXPECT_THROW(INI_UNUSED(file["Section"]["d"].to<bool>()), ini::DecodeError);
}
