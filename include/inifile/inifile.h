#pragma once

#include <charconv>
#include <filesystem>
#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>

namespace ini
{

template <typename T, typename = std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>>>
T decode(std::string const& str)
{
    T number{};
    auto [ptr, ec] = std::from_chars(str.begin().base(), str.end().base(), number);

    switch (ec)
    {
        case std::errc::invalid_argument:    // TODO (SuniRein): Process Error; case std::errc::result_out_of_range: ;
        case std::errc::result_out_of_range:

        default:
            return number;
    }
}

/**
 * An interface to ini file.
 * It can convert to any c++ type easily if a converter is defined.
 */
class Field
{
  public:
    Field() = default;

    explicit Field(std::string value): value_(std::move(value)) {}
    Field& operator=(std::string value)
    {
        value_ = std::move(value);
        return *this;
    }

    explicit Field(std::string_view value): value_(value) {}
    Field& operator=(std::string_view value)
    {
        value_ = value;
        return *this;
    }

    explicit Field(char const* value): value_(value) {}
    Field& operator=(char const* value)
    {
        value_ = value;
        return *this;
    }

    /// Get the inner string as `std::string`.
    /// It is **cost-free**.
    [[nodiscard]]
    std::string const& as_str() const { return value_; }

    /// Get the inner string as `char const*`.
    /// It is **cost-free**.
    [[nodiscard]]
    char const* as_cstr() const { return value_.c_str(); }

    /// Convert into type `T`.
    template <typename T>
    [[nodiscard]]
    T to()
    {
        return decode<T>(value_);
    }

  private:
    std::string value_;
};

/**
 * Process ini section.
 */
class Section: public std::map<std::string, Field> {};

/**
 * Core process class.
 */
class File: public std::map<std::string, Section>
{
  public:
    /// Read file from path and decode it.
    /// Return false iff error happen.
    /// Run File::error() for mare information.
    bool read(std::filesystem::path const& file);

    /// Write to a file.
    /// Return false iff error happen.
    /// Run File::error() for mare information.
    bool write(std::filesystem::path const& file) const;

    /// Read from a string and decode it.
    /// Return false iff error happen.
    /// Run File::error() for mare information.
    bool decode(std::string_view str);

    /// Write to a string.
    [[nodiscard]]
    std::string encode() const;

    /// Read from std::istream and decode it.
    /// Return false iff error happen.
    /// Run File::error() for mare information.
    bool decode(std::istream& input);

    /// Write to std::ostream.
    void encode(std::ostream& output) const;

    /// Get the detailed error description.
    [[nodiscard]]
    std::string_view eroor() const { return error_; }

  private:
    mutable std::string error_;
};

} // namespace ini
