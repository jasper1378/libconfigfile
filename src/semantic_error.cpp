#include "semantic_error.hpp"

#include <exception>
#include <stdexcept>
#include <string>
#include <utility>

conf_file::semantic_error::semantic_error(const std::string& what_arg)
    : std::runtime_error{ what_arg }
{
}

conf_file::semantic_error::semantic_error(const char* what_arg)
    : std::runtime_error{ what_arg }
{
}

conf_file::semantic_error::semantic_error(const semantic_error& other) noexcept
    : std::runtime_error{ other }
{
}

conf_file::semantic_error::semantic_error(semantic_error&& other) noexcept
    : std::runtime_error{ std::move(other ) }
{
}

conf_file::semantic_error::~semantic_error()
{
}

const char* conf_file::semantic_error::what() const noexcept
{
    return std::runtime_error::what();
}

conf_file::semantic_error& conf_file::semantic_error::operator=(const semantic_error& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    std::runtime_error::operator=(other);

    return *this;
}

conf_file::semantic_error& conf_file::semantic_error::operator=(semantic_error&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    std::runtime_error::operator=(std::move(other));

    return *this;
}

conf_file::semantic_error conf_file::semantic_error::generate_formatted_error(const std::string& file_name, const size_t pos_line, const size_t pos_char, const std::string& what_arg)
{
    return semantic_error{ file_name + ":" + std::to_string(pos_line) + ":" + std::to_string(pos_char) + ": " + what_arg };
}

conf_file::semantic_error conf_file::semantic_error::generate_formatted_error(const file& f, const file_pos& pos, const std::string& what_arg)
{
    return generate_formatted_error(f.get_file_path(), pos.get_line(), pos.get_char(), what_arg);
}

conf_file::semantic_error conf_file::semantic_error::generate_formatted_error(const std::string& file_name, const file_pos& pos, const std::string& what_arg)
{
    return generate_formatted_error(file_name, pos.get_line(), pos.get_char(), what_arg);
}

conf_file::semantic_error conf_file::semantic_error::generate_formatted_error(const file& f, const size_t pos_line, const size_t pos_char, const std::string& what_arg)
{
    return generate_formatted_error(f.get_file_path(), pos_line, pos_char, what_arg);
}
