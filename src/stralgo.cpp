#include "stralgo.h"

#include <string_view>

namespace
{
    using SizeType = std::string_view::size_type;

    /// substr() method for std::string_view with [begin, end).
    std::string_view substr(std::string_view str, SizeType begin, SizeType end)
    {
        return str.substr(begin, end - begin);
    }

    constexpr std::string_view BLANK_CHARS {" \t\r\n"};
} // anonymous namespace

namespace ini::str
{

std::string_view extract_section_name(std::string_view str)
{
    str = trim(str);
    if (str.size() > 2 && str.front() == '[' && str.back() == ']')
    {
        return trim(substr(str, 1, str.size() - 1));
    }
    return std::string_view{};
}

KeyValue extract_key_value(std::string_view str)
{
    if (auto pos = str.find_first_of('='); pos != std::string_view::npos)
    {
        return KeyValue{
            .key   = trim(substr(str, 0, pos)),
            .value = trim(substr(str, pos + 1, str.size() + 1)),
        };
    }
    return KeyValue{};
}

bool is_empty_line(std::string_view str)
{
    return str.find_first_not_of(BLANK_CHARS) == std::string_view::npos;
}

std::string_view trim(std::string_view str)
{
    auto begin = str.find_first_not_of(BLANK_CHARS);
    auto before_end = str.find_last_not_of(BLANK_CHARS);

    if (begin == std::string_view::npos)
    {
        return std::string_view{};
    }
    return substr(str, begin, before_end + 1);
}

} // namespace ini::str
