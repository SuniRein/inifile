#pragma once

#include <string_view>

namespace ini::str
{

/// Judge if str is of mode "[section]".
/// If true, return the name of the section.
/// If false, return an empty string.
[[nodiscard]]
std::string_view extract_section_name(std::string_view str);

struct KeyValue
{
    std::string_view key;
    std::string_view value;
};
/// Judge if str is of mode "key=value"
/// If true, return the key-value pair.
/// If false, return two empty string.
[[nodiscard]]
KeyValue extract_key_value(std::string_view str);

/// Erase all comments in the line,
/// which begins with '#' or ';'.
[[nodiscard]]
std::string_view erase_comments(std::string_view str);

/// Jedge if a line is made of blank char.
[[nodiscard]]
bool is_empty_line(std::string_view str);

/// Remove blank chars beside.
[[nodiscard]]
std::string_view trim(std::string_view str);

} // namespace ini::str
