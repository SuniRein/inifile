#pragma once

#include <filesystem>
#include <istream>
#include <map>
#include <ostream>
#include <string>

namespace ini
{

/**
 * Process ini section.
 */
class Section: private std::map<std::string, std::string>
{
  public:
    using Base = std::map<std::string, std::string>;

    // Iterator supports.
    using Base::begin;   using Base::end;
    using Base::cbegin;  using Base::cend;
    using Base::rbegin;  using Base::rend;
    using Base::crbegin; using Base::crend;

    // Access supports;
    using Base::operator[];
    using Base::at;
    using Base::find;

    Section() = default;

    // Copying need to be implemented for the buffer.
    Section(Section const&);
    Section& operator=(Section const&);

    /// Encode the section into buffer.
    void encode();

    /// Decode the section from buffer.
    void decode();

  private:
    /// The buffer is owned by its parent File.
    std::string_view buffer_;
};

/**
 * Core process class.
 */
class File: private std::map<std::string, Section>
{
  public:
    using Base = std::map<std::string, Section>;

    // Iterator supports.
    using Base::begin;   using Base::end;
    using Base::cbegin;  using Base::cend;
    using Base::rbegin;  using Base::rend;
    using Base::crbegin; using Base::crend;

    // Access supports;
    using Base::operator[];
    using Base::at;
    using Base::find;

    /// Read file from path but not decode.
    /// You need to decode it before accessing!
    void read(std::filesystem::path const& input_file);

    /// Write to file without auto encode.
    /// You need to encode it before!
    void write(std::filesystem::path const& output_file);

    /// Encode the current sections.
    void encode();

    /// Decode the file you read.
    void decode();

    /// Encode and write to std::ostream.
    void encode(std::istream& input);

    /// Read from std::ostream and decode.
    void decode(std::ostream& output);

  private:
    std::string buffer_;
};

} // namespace ini
