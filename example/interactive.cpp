#include <filesystem>
#include <iostream>
#include <istream>
#include <map>
#include <string>
#include <string_view>

#include "inifile/inifile.h"

ini::File parser;

void error(bool success, std::string_view location)
{
    if (!success)
    {
        std::cerr << "Error at " << location << ": " << parser.eroor() << "\n";
    }
}

void read(std::istream& input)
{
    std::filesystem::path path;
    input >> path;
    error(parser.read(path), "function read()");
}

void write(std::istream& input)
{
    std::filesystem::path path;
    input >> path;
    error(parser.write(path), "function write()");
}

void find(std::istream& input)
{
    std::string section_name;
    std::string key_name;
    input >> section_name >> key_name;

    auto section = parser.find(section_name);
    if (section == parser.end())
    {
        std::cerr << "Unkonwn section name.\n";
        return;
    }

    auto key = section->second.find(key_name);
    if (key == section->second.end())
    {
        std::cerr << "Unkonwn key name\n";
        return;
    }

    std::cout << "[" << section_name << "] " << key_name << ": " << key->second << "\n";
}

void update(std::istream& input)
{
    std::string section_name;
    std::string key_name;
    std::string value;
    input >> section_name >> key_name >> value;

    parser[section_name][key_name] = value;
}

int main()
{
    using Command = void(*)(std::istream&);

    std::map<std::string, Command> commands{
        {"read", read},
        {"write", write},
        {"find", find},
        {"update", update},
    };

    std::string command;
    while (std::cin >> command)
    {
        if (commands.find(command) != commands.end())
        {
            commands[command](std::cin);
        }
        else
        {
            std::cerr << "Unknown command.\n";
        }
    }
}

