#include "inifile/inifile.h"

#include "gtest/gtest.h"

using namespace std::string_view_literals;

constexpr auto INI_FILE = R"(
[Section 1]
name=git
blog=http://git.github.com
  

[ Section 2  ]
user = suni
    money = 100
    industry =s=j

)"sv;

constexpr auto INI_FILE_ENCODED = 
R"([Section 1]
blog = http://git.github.com
name = git

[Section 2]
industry = s=j
money = 100
user = suni

[Section 3]
Happy = yes
new = try

)";

TEST(IniFile, TEST)
{
    ini::File parser;

    ASSERT_TRUE(parser.decode(INI_FILE));

    EXPECT_EQ(parser["Section 1"]["name"],     "git"sv);
    EXPECT_EQ(parser["Section 1"]["blog"],     "http://git.github.com"sv);

    EXPECT_EQ(parser["Section 2"]["user"],     "suni"sv);
    EXPECT_EQ(parser["Section 2"]["money"],    "100"sv);
    EXPECT_EQ(parser["Section 2"]["industry"], "s=j"sv);

    parser["Section 3"]["new"] = "try";
    parser["Section 3"]["Happy"] = "yes";

    EXPECT_EQ(parser.encode(), INI_FILE_ENCODED);
}
