#include "config.hpp"

#include "array_value_node.hpp"
#include "end_value_node.hpp"
#include "float_end_value_node.hpp"
#include "integer_end_value_node.hpp"
#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"
#include "section_node.hpp"
#include "semantic_error.hpp"
#include "string_end_value_node.hpp"
#include "syntax_error.hpp"
#include "value_node.hpp"

#include <cctype>
#include <cstddef>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

libconfigfile::config::config()
    : m_raw_file_contents{},
      m_values{}
{
}

libconfigfile::config::config(const std::string& file_name)
    : m_raw_file_contents{ file_name, false },
      m_values{}
{
    parse_file();
}

libconfigfile::config::config(const config& other)
    : m_raw_file_contents{ other.m_raw_file_contents },
      m_values{ other.m_values }
{
}

libconfigfile::config::config(config&& other)
    : m_raw_file_contents{ std::move(other.m_raw_file_contents) },
      m_values{ std::move(other.m_values) }
{
}

libconfigfile::config::~config()
{
}

libconfigfile::config& libconfigfile::config::operator=(const config& other)
{
    if (this == &other)
    {
        return *this;
    }

    m_raw_file_contents = other.m_raw_file_contents;
    m_values = other.m_values;

    return *this;
}

libconfigfile::config& libconfigfile::config::operator=(config&& other)
{
    if (this == &other)
    {
        return *this;
    }

    m_raw_file_contents = std::move(other.m_raw_file_contents);
    m_values = std::move(other.m_values);

    return *this;
}

void libconfigfile::config::parse_file()
{
    static const auto is_whitespace{ [](char c)
    {
        for (size_t i{ 0 }; i < m_k_whitespace_chars.size(); ++i)
        {
            if (c == m_k_whitespace_chars[i])
            {
                return true;
            }
        }

        return false;
    } };

    file_pos cur_pos{ m_raw_file_contents.create_file_pos() };

    while (cur_pos.is_eof() == false)
    {
        cur_pos.goto_end_of_whitespace(m_k_whitespace_chars);

        if (m_raw_file_contents[cur_pos] == m_k_comment_script)
        {
            cur_pos.goto_next_line();
        }
        else if () //c/c++ comments
        {
        }
        else if (m_raw_file_contents[cur_pos] == m_k_comment_leader)
        {
            file_pos next_pos{ cur_pos + 1 };

            if (m_raw_file_contents[next_pos] == m_k_single_line_comment[1])
            {
                cur_pos.goto_next_line();
            }
            else if (m_raw_file_contents[next_pos] == m_k_multi_line_comment_start[1])
            {
                cur_pos.goto_find_end(m_k_multi_line_comment_end);
            }
            else
            {
                std::string what_arg{ std::string{ "random character '" } + m_raw_file_contents[cur_pos] + "'" };
                throw syntax_error::generate_formatted_error(m_raw_file_contents, cur_pos, what_arg);
            }
        }
        else if (m_raw_file_contents[cur_pos] == m_k_directive_leader)
        {
            file_pos test_pos{ cur_pos - 1 };
            test_pos.goto_find_last_not_of(m_k_whitespace_chars);
            if ((test_pos.get_line() == cur_pos.get_line()) && (test_pos.is_bof() == false))
            {
                std::string what_arg{ "directive must be the only text on its line" };
                throw syntax_error::generate_formatted_error(m_raw_file_contents, cur_pos, what_arg);
            }

            parse_directive(cur_pos);
            //TODO continue from here
        }
    }
}

void libconfigfile::config::parse_directive(file_pos& cur_pos)
{
    std::string directive_line{ m_raw_file_contents.get_line(cur_pos) };
    std::string::size_type line_pos{ cur_pos.get_char() };

    ++line_pos;
    line_pos = directive_line.find_first_not_of(m_k_whitespace_chars, line_pos);
    if (line_pos == std::string::npos)
    {
        std::string what_arg{ "expected directive name" };
        throw syntax_error::generate_formatted_error(m_raw_file_contents, cur_pos.get_line(), line_pos, what_arg);
    }
    std::string::size_type start_of_name{ line_pos };

    line_pos = directive_line.find_first_of(m_k_whitespace_chars, line_pos);
    if (line_pos == std::string::npos)
    {
        line_pos = (directive_line.size() - 1);
    }
    std::string::size_type end_of_name{ line_pos };

    std::string name{ get_substr_between_indices(directive_line, start_of_name, end_of_name) };

    std::string args{};
    std::string::size_type start_of_args{};
    ++line_pos;
    line_pos = directive_line.find_first_not_of(m_k_whitespace_chars, line_pos);
    if (line_pos == std::string::npos)
    {
        args = "";
    }
    else
    {
        start_of_args = line_pos;
        args = get_substr_between_indices(directive_line, start_of_args, (directive_line.size() - 1));
    }

    cur_pos.set_char(start_of_args);

    if (name == "include")
    {
        parse_include_directive(cur_pos, args);
    }
    else
    {
        std::string what_arg{ "unknown directive" };
        throw syntax_error::generate_formatted_error(m_raw_file_contents, cur_pos.get_line(), start_of_name, what_arg);
    }
}

void libconfigfile::config::parse_include_directive(file_pos& cur_pos, const std::string& args)
{
    if (args[0] != '"')
    {
        std::string what_arg{ "include directive requires file path argument" };
    }
    //TODO continue from here
}

bool libconfigfile::config::is_pos_located_on_occurence_of(const file_pos& pos, const std::string& str)
{
    return ((pos.get_char()) == (m_raw_file_contents.get_line(pos).find(str, pos.get_char())));
}

std::string libconfigfile::config::get_substr_between_indices(const std::string& str, const std::string::size_type start, const std::string::size_type end)
{
    return str.substr((start), ((end - start) + 1));
}
