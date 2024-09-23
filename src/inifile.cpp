#include "inifile/inifile.h"

#include <fstream>
#include <sstream>
#include <string>

#include "stralgo.h"

namespace ini
{

bool File::read(std::filesystem::path const& file)
{
    std::ifstream stream(file);
    if (!stream.is_open())
    {
        error_ = "Failed to open file at " + file.string();
        return false;
    }

    return decode(stream);
}

bool File::write(std::filesystem::path const& file) const
{
    std::ofstream stream(file);
    if (!stream.is_open())
    {
        error_ = "Failed to open file at " + file.string();
        return false;
    }

    encode(stream);
    return true;
}

bool File::decode(std::string_view str)
{
    std::stringstream stream;
    stream << str;
    return decode(stream);
}

std::string File::encode() const
{
    std::stringstream stream;
    encode(stream);
    return stream.str();
}

// TODO: Two empty line in the end.
void File::encode(std::ostream& output) const
{
    for (auto const& [name, section] : *this)
    {
        // Section name.
        output << "[" << name << "]\n";

        // Section body.
        for (auto const& [key, value] : section)
        {
            output << key << " = " << value << "\n";
        }

        // Empty line after each section.
        output << "\n";
    }
}

bool File::decode(std::istream& input)
{
    std::string buffer;
    std::string current_section;
    int line = 0; // record line number.

    while (std::getline(input, buffer))
    {
        ++line;

        auto processed_str = str::erase_comments(buffer);

        // Process section.
        if (auto name = str::extract_section_name(processed_str); !name.empty())
        {
            current_section = name;
            continue;
        }

        // Process key-value.
        if (auto [key, value] = str::extract_key_value(processed_str); !key.empty())
        {
            if (current_section.empty())
            {
                error_ = "Syntax error at line " + std::to_string(line) + ": Expected a section name";
                return false;
            }

            (*this)[current_section][std::string(key)] = value;
            continue;
        }

        // Process error line.
        if (!str::is_empty_line(processed_str))
        {
            error_ = "Syntax error at line " + std::to_string(line);
            return false;
        }
    }
    return true;
}

} // namespace ini
