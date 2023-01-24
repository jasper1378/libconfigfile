#include "file.hpp"

#include <cstddef>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

conf_file::file::file()
    : m_file_path{},
      m_file_contents{}
{
}

conf_file::file::file(const std::string& file_path, bool insert_newlines/*= true*/)
    : m_file_path{ file_path },
      m_file_contents{ read_file(m_file_path, insert_newlines) }
{
}

conf_file::file::file(std::string&& file_path, bool insert_newlines/*= true*/)
    : m_file_path{ std::move(file_path) },
      m_file_contents{ read_file(m_file_path, insert_newlines) }
{
}

conf_file::file::file(const file& other)
    : m_file_path{ other.m_file_path },
      m_file_contents{ other.m_file_contents }
{
}

conf_file::file::file(file&& other)
    : m_file_path{ std::move(other.m_file_path) },
      m_file_contents{ std::move(other.m_file_contents) }
{
}

conf_file::file::~file()
{
}

bool conf_file::file::is_paired(const file_pos& pos) const
{
    return (this == pos.get_paired_file());
}

std::string conf_file::file::get_file_path() const
{
    return m_file_path;
}

conf_file::file_pos conf_file::file::create_file_pos() const
{
    return file_pos{ this };
}

const char& conf_file::file::get_char(const file_pos& pos) const
{
    if (pos.get_paired_file() != this)
    {
        throw std::runtime_error{ "given file_pos is not paired with this file" };
    }

    if (pos.is_eof() == true)
    {
        throw std::runtime_error{ "given file_pos is end-of-file" };
    }

    if (pos.is_bof() == true)
    {
        throw std::runtime_error{ "give file_pos is beginning-of-file" };
    }

    return m_file_contents[pos.get_line()][pos.get_char()];
}

const std::string& conf_file::file::get_line(const file_pos& pos) const
{
    if (pos.get_paired_file() != this)
    {
        throw std::runtime_error{ "given file_pos is not paired with this file" };
    }

    if (pos.is_eof() == true)
    {
        throw std::runtime_error{ "given file_pos is end-of-file" };
    }

    if (pos.is_bof() == true)
    {
        throw std::runtime_error{ "given file_pos is beginning-of-file" };
    }

    return m_file_contents[pos.get_line()];
}

const std::vector<std::string>& conf_file::file::get_array() const
{
    return m_file_contents;
}

std::vector<std::string>& conf_file::file::get_underlying()
{
    return m_file_contents;
}

conf_file::file& conf_file::file::operator=(const file& other)
{
    if (this == &other)
    {
        return *this;
    }

    m_file_path = other.m_file_path;
    m_file_contents = other.m_file_contents;

    return *this;
}

conf_file::file& conf_file::file::operator=(file&& other)
{
    if (this == &other)
    {
        return *this;
    }

    m_file_path = std::move(other.m_file_path);
    m_file_contents = std::move(other.m_file_contents);

    return *this;
}

const char& conf_file::file::operator[](const file_pos& pos) const
{
    if (pos.get_paired_file() != this)
    {
        throw std::runtime_error{ "given file_pos is not paired with this file" };
    }

    if (pos.is_eof() == true)
    {
        throw std::runtime_error{ "given file_pos is end-of-file" };
    }

    if (pos.is_bof() == true)
    {
        throw std::runtime_error{ "give file_pos is beginning-of-file" };
    }

    return m_file_contents[pos.get_line()][pos.get_char()];
}

std::vector<std::string> conf_file::file::read_file(const std::string& file_path, const bool insert_newlines/*= false*/)
{
    std::ifstream input_file{ file_path };

    if (!input_file)
    {
        throw std::runtime_error{ "file \"" + file_path + "\" could not be opened for reading" };
    }

    std::vector<std::string> file_contents{};

    while (input_file)
    {
        std::string new_line{};
        std::getline(input_file, new_line);

        if (insert_newlines == true)
        {
            new_line.push_back(m_k_newline);
        }

        file_contents.push_back(new_line);
    }

    return file_contents;
}

conf_file::file_pos::file_pos(const file* file_in_which_to_move)
    : m_file{ file_in_which_to_move },
      m_line{ 0 },
      m_char{ 0 },
      m_bof{ false },
      m_eof{ false }
{
    if (file_in_which_to_move == nullptr)
    {
        throw std::runtime_error{ "file cannot be null" };
    }
}

conf_file::file_pos::file_pos(const file_pos& other)
    : m_file{ other.m_file },
      m_line{ other.m_line },
      m_char{ other.m_char },
      m_bof{ other.m_bof },
      m_eof{ other.m_eof }
{
}

conf_file::file_pos::file_pos(file_pos&& other)
    : m_file{ std::move(other.m_file) },
      m_line{ std::move(other.m_line) },
      m_char{ std::move(other.m_char) },
      m_bof{ std::move(other.m_bof) },
      m_eof{ std::move(other.m_eof) }
{
}

conf_file::file_pos::~file_pos()
{
}

bool conf_file::file_pos::is_paired(const file& f) const
{
    return m_file == &f;
}

const conf_file::file* conf_file::file_pos::get_paired_file() const
{
    return m_file;
}

bool conf_file::file_pos::is_bof() const
{
    return m_bof;
}

bool conf_file::file_pos::is_eof() const
{
    return m_eof;
}

size_t conf_file::file_pos::get_line() const
{
    return m_line;
}

size_t conf_file::file_pos::get_char() const
{
    return m_char;
}

void conf_file::file_pos::set_line(const size_t line_val)
{
    if (line_val >= m_file->get_array().size())
    {
        throw std::runtime_error{ "line value is out of range" };
    }
    else
    {
        m_line = line_val;
    }
}

void conf_file::file_pos::set_char(const size_t char_val)
{
    if (char_val >= m_file->get_line(*this).size())
    {
        throw std::runtime_error{ "char value is out of range" };
    }
    else
    {
        m_char = char_val;
    }
}

void conf_file::file_pos::goto_next_line(size_t lines_to_move/*= 1*/)
{
    if (lines_to_move == 0)
    {
        return;
    }
    else
    {
        if (m_eof == true)
        {
            return;
        }
        else if ((m_line + lines_to_move) < (m_file->get_array().size()))
        {
            m_line += lines_to_move;
            m_char = 0;
            m_bof = false;
        }
        else
        {
            set_eof();
            m_bof = false;
        }
    }
}

void conf_file::file_pos::goto_prev_line(size_t lines_to_move/*= 1*/)
{
    if (lines_to_move == 0)
    {
        return;
    }
    else
    {
        if (m_bof == true)
        {
            return;
        }
        else if ((lines_to_move) <= (m_line))
        {
            m_line -= lines_to_move;
            m_char = 0;
            m_eof = false;
        }
        else
        {
            set_bof();
            m_eof = false;
        }
    }
}

void conf_file::file_pos::goto_next_char(size_t chars_to_move/*= 1*/)
{
    while (true)
    {
        if (m_eof == true)
        {
            return;
        }
        else if ((m_char + chars_to_move) < (m_file->get_line(*this).size()))
        {
            m_char += chars_to_move;
            chars_to_move = 0;
            m_bof = false;
            return;
        }
        else
        {
            chars_to_move -= (m_file->get_line(*this).size() - m_char);
            goto_next_line();
        }
    }
}

void conf_file::file_pos::goto_prev_char(size_t chars_to_move/*= 1*/)
{
    while (true)
    {
        if (m_bof == true)
        {
            return;
        }
        else if ((chars_to_move) <= (m_char))
        {
            m_char -= chars_to_move;
            chars_to_move = 0;
            m_eof = false;
            return;
        }
        else
        {
            size_t temp_char{ m_char };
            goto_prev_line();
            m_char -= (m_file->get_line(*this).size() + temp_char);
        }
    }
}

void conf_file::file_pos::goto_find_start(const std::string& to_find)
{
    if (m_eof == true)
    {
        return;
    }

    while (true)
    {
        std::string::size_type pos{ m_file->get_line(*this).find(to_find, m_char) };

        if (pos == std::string::npos)
        {
            goto_next_line();

            if (m_eof == true)
            {
                return;
            }
            else
            {
                continue;
            }
        }
        else
        {
            m_char = pos;
            return;
        }
    }
}

void conf_file::file_pos::goto_find_end(const std::string& to_find)
{
    if (m_eof == true)
    {
        return;
    }

    goto_find_start(to_find);
    goto_next_char(to_find.size());
}

void conf_file::file_pos::goto_rfind_start(const std::string& to_find)
{
    if (m_bof == true)
    {
        return;
    }

    while (true)
    {
        std::string::size_type pos{ m_file->get_line(*this).rfind(to_find, m_char) };

        if (pos == std::string::npos)
        {
            goto_prev_line();

            if (m_bof == true)
            {
                return;
            }
            else
            {
                continue;
            }
        }
        else
        {
            m_char = pos;
            return;
        }
    }
}

void conf_file::file_pos::goto_rfind_end(const std::string& to_find)
{
    if (m_bof == true)
    {
        return;
    }

    goto_rfind_start(to_find);
    goto_next_char(to_find.size());
}

void conf_file::file_pos::goto_find_first_of(const std::string& to_find)
{
    if (m_eof == true)
    {
        return;
    }

    while (true)
    {
        std::string::size_type pos{ m_file->get_line(*this).find_first_of(to_find, m_char) };

        if (pos == std::string::npos)
        {
            goto_next_line();

            if (m_eof == true)
            {
                return;
            }
            else
            {
                continue;
            }
        }
        else
        {
            m_char = pos;
            return;
        }
    }
}

void conf_file::file_pos::goto_find_first_of(const std::vector<char>& to_find)
{
    goto_find_first_of(std::string{ to_find.begin(), to_find.end() });
}


void conf_file::file_pos::goto_find_first_not_of(const std::string& to_find)
{
    if (m_eof == true)
    {
        return;
    }

    while (true)
    {
        std::string::size_type pos{ m_file->get_line(*this).find_first_not_of(to_find, m_char) };

        if (pos == std::string::npos)
        {
            goto_next_line();

            if (m_eof == true)
            {
                return;
            }
            else
            {
                continue;
            }
        }
        else
        {
            m_char = pos;
            return;
        }
    }
}

void conf_file::file_pos::goto_find_first_not_of(const std::vector<char>& to_find)
{
    goto_find_first_not_of(std::string{ to_find.begin(), to_find.end() });
}

void conf_file::file_pos::goto_find_last_of(const std::string& to_find)
{
    if (m_bof == true)
    {
        return;
    }

    while (true)
    {
        std::string::size_type pos{ m_file->get_line(*this).find_last_of(to_find, m_char) };

        if (pos == std::string::npos)
        {
            goto_prev_line();

            if (m_bof == true)
            {
                return;
            }
            else
            {
                continue;
            }
        }
        else
        {
            m_char = pos;
            return;
        }
    }
}

void conf_file::file_pos::goto_find_last_of(const std::vector<char>& to_find)
{
    goto_find_last_of(std::string{ to_find.begin(), to_find.end() });
}

void conf_file::file_pos::goto_find_last_not_of(const std::string& to_find)
{
    if (m_bof == true)
    {
        return;
    }

    const std::string to_find_str{ to_find.begin(), to_find.end() };

    while (true)
    {
        std::string::size_type pos{ m_file->get_line(*this).find_last_not_of(to_find_str, m_char) };

        if (pos == std::string::npos)
        {
            goto_prev_line();

             if (m_bof == true)
             {
                 return;
             }
             else
             {
                 continue;
             }
        }
        else
        {
            m_char = pos;
            return;
        }
    }
}

void conf_file::file_pos::goto_find_last_not_of(const std::vector<char>& to_find)
{
    goto_find_last_not_of(std::string{ to_find.begin(), to_find.end() });
}

void conf_file::file_pos::goto_end_of_whitespace(const std::string& whitespace_chars/*= " \t"*/)
{
    if (m_eof == true)
    {
        return;
    }

    static const auto is_whitespace{ [&whitespace_chars](char c) -> bool
    {
        for (size_t i{ 0 }; i < whitespace_chars.size(); ++i)
        {
            if (c == whitespace_chars[i])
            {
                return true;
            }
        }

        return false;
    } };

    if (is_whitespace(m_file->get_char(*this)) == false)
    {
        return;
    }

    while (true)
    {
        std::string::size_type pos{ m_file->get_line(*this).find_first_not_of(whitespace_chars, m_char) };

        if (pos == std::string::npos)
        {
            goto_next_line();

            if (m_eof == true)
            {
                return;
            }
            else
            {
                continue;
            }
        }
        else
        {
            m_char = pos;
            return;
        }
    }
}

void conf_file::file_pos::goto_end_of_whitespace(const std::vector<char>& whitespace_chars)
{
    goto_end_of_whitespace(std::string{ whitespace_chars.begin(), whitespace_chars.end() });
}

void conf_file::file_pos::goto_start_of_whitespace(const std::string& whitespace_chars/*= " \t"*/)
{
    if (m_bof == true)
    {
        return;
    }

    static const auto is_whitespace{ [&whitespace_chars](char c) -> bool
    {
        for (size_t i{ 0 }; i < whitespace_chars.size(); ++i)
        {
            if (c == whitespace_chars[i])
            {
                return true;
            }
        }

        return false;
    } };

    if (is_whitespace(m_file->get_char(*this)) == false)
    {
        return;
    }

    while (true)
    {
        std::string::size_type pos{ m_file->get_line(*this).find_last_not_of(whitespace_chars, m_char) };

        if (pos == std::string::npos)
        {
            goto_prev_line();

            if (m_bof == true)
            {
                return;
            }
            else
            {
                continue;
            }
        }
        else
        {
            m_char = pos;
            return;
        }
    }
}

void conf_file::file_pos::goto_start_of_whitespace(const std::vector<char>& whitespace_chars)
{
    goto_start_of_whitespace(std::string{ whitespace_chars.begin(), whitespace_chars.end() });
}

conf_file::file_pos& conf_file::file_pos::operator=(const file_pos& other)
{
    if (this == &other)
    {
        return *this;
    }

    m_file = other.m_file;
    m_line = other.m_line;
    m_char = other.m_char;
    m_bof = other.m_bof;
    m_eof = other.m_eof;

    return *this;
}

conf_file::file_pos& conf_file::file_pos::operator=(file_pos&& other)
{
    if (this == &other)
    {
        return *this;
    }

    m_file = std::move(other.m_file);
    m_line = std::move(other.m_line);
    m_char = std::move(other.m_char);
    m_bof = std::move(other.m_bof);
    m_eof = std::move(other.m_eof);

    return *this;
}

bool conf_file::file_pos::operator==(const file_pos& other) const
{
    return ((m_line == other.m_line) && (m_char == other.m_char));
}

bool conf_file::file_pos::operator!=(const file_pos& other) const
{
    return (!(operator==(other)));
}

conf_file::file_pos& conf_file::file_pos::operator++()
{
    goto_next_char();
    return *this;
}

conf_file::file_pos conf_file::file_pos::operator++(int)
{
    file_pos temp{ *this };
    operator++();
    return temp;
}

conf_file::file_pos& conf_file::file_pos::operator--()
{
    goto_prev_char();
    return *this;
}

conf_file::file_pos conf_file::file_pos::operator--(int)
{
    file_pos temp{ *this };
    operator--();
    return temp;
}

conf_file::file_pos& conf_file::file_pos::operator+=(const size_t chars_to_move)
{
    goto_next_char(chars_to_move);
    return *this;
}

conf_file::file_pos& conf_file::file_pos::operator-=(const size_t chars_to_move)
{
    goto_prev_char(chars_to_move);
    return *this;
}

void conf_file::file_pos::set_bof()
{
    m_bof = true;
    m_line = 0;
    m_char = 0;
}

void conf_file::file_pos::set_eof()
{
    m_eof = true;
    m_line = (m_file->get_array().size() - 1);
    m_char = (m_file->get_line(*this).size() - 1);
}

conf_file::file_pos conf_file::operator+(file_pos pos, const size_t chars_to_move)
{
    pos.goto_next_char(chars_to_move);
    return pos;
}

conf_file::file_pos conf_file::operator+(const size_t chars_to_move, file_pos pos)
{
    pos.goto_next_char(chars_to_move);
    return pos;
}

conf_file::file_pos conf_file::operator-(file_pos pos, const size_t chars_to_move)
{
    pos.goto_prev_char(chars_to_move);
    return pos;
}

conf_file::file_pos conf_file::operator-(const size_t chars_to_move, file_pos pos)
{
    pos.goto_prev_char(chars_to_move);
    return pos;
}
