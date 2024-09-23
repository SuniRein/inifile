#pragma once

#include <filesystem>
#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <string_view>

namespace ini
{

/**
 * Process ini section.
 */
class Section: public std::map<std::string, std::string> {};

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
