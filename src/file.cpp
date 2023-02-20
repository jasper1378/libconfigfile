#include "file.hpp"

#include <cstddef>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

libconfigfile::file::file() : m_file_path{}, m_file_contents{} {}

libconfigfile::file::file(const std::string &file_path,
                          bool insert_newlines /*= true*/)
    : m_file_path{file_path}, m_file_contents{
                                  read_file(m_file_path, insert_newlines)} {}

libconfigfile::file::file(std::string &&file_path,
                          bool insert_newlines /*= true*/)
    : m_file_path{std::move(file_path)}, m_file_contents{read_file(
                                             m_file_path, insert_newlines)} {}

libconfigfile::file::file(const file &other)
    : m_file_path{other.m_file_path}, m_file_contents{other.m_file_contents} {}

libconfigfile::file::file(file &&other) noexcept
    : m_file_path{std::move(other.m_file_path)}, m_file_contents{std::move(
                                                     other.m_file_contents)} {}

libconfigfile::file::~file() {}

bool libconfigfile::file::is_paired(const file_pos &pos) const {
  return (this == pos.get_paired_file());
}

std::string libconfigfile::file::get_file_path() const { return m_file_path; }

libconfigfile::file_pos libconfigfile::file::create_file_pos() const {
  return file_pos{this};
}

libconfigfile::file_pos
libconfigfile::file::create_file_pos(const file_pos &start_pos) const {
  return file_pos{this, start_pos};
}

const char &libconfigfile::file::get_char(const file_pos &pos) const {
  if (pos.get_paired_file() != this) {
    throw std::runtime_error{"given file_pos is not paired with "
                             "this file"};
  }

  if (pos.is_eof() == true) {
    throw std::runtime_error{"given file_pos is end-of-file"};
  }

  if (pos.is_bof() == true) {
    throw std::runtime_error{"give file_pos is "
                             "beginning-of-file"};
  }

  return m_file_contents[pos.get_line()][pos.get_char()];
}

const std::string &libconfigfile::file::get_line(const file_pos &pos) const {
  if (pos.get_paired_file() != this) {
    throw std::runtime_error{"given file_pos is not paired with "
                             "this file"};
  }

  if (pos.is_eof() == true) {
    throw std::runtime_error{"given file_pos is end-of-file"};
  }

  if (pos.is_bof() == true) {
    throw std::runtime_error{"given file_pos is "
                             "beginning-of-file"};
  }

  return m_file_contents[pos.get_line()];
}

const std::vector<std::string> &libconfigfile::file::get_array() const {
  return m_file_contents;
}

std::vector<std::string> &libconfigfile::file::get_underlying() {
  return m_file_contents;
}

libconfigfile::file &libconfigfile::file::operator=(const file &other) {
  m_file_path = other.m_file_path;
  m_file_contents = other.m_file_contents;

  return *this;
}

libconfigfile::file &libconfigfile::file::operator=(file &&other) noexcept(
    (std::is_nothrow_assignable_v<
        decltype(m_file_path),
        decltype(m_file_path)>)&&(std::
                                      is_nothrow_assignable_v<
                                          decltype(m_file_contents),
                                          decltype(m_file_contents)>)) {
  m_file_path = std::move(other.m_file_path);
  m_file_contents = std::move(other.m_file_contents);

  return *this;
}

const char &libconfigfile::file::operator[](const file_pos &pos) const {
  if (pos.get_paired_file() != this) {
    throw std::runtime_error{"given file_pos is not paired with "
                             "this file"};
  }

  if (pos.is_eof() == true) {
    throw std::runtime_error{"given file_pos is end-of-file"};
  }

  if (pos.is_bof() == true) {
    throw std::runtime_error{"give file_pos is "
                             "beginning-of-file"};
  }

  return m_file_contents[pos.get_line()][pos.get_char()];
}

std::vector<std::string>
libconfigfile::file::read_file(const std::string &file_path,
                               const bool insert_newlines /*= false*/) {
  std::ifstream input_file{file_path};

  if (!input_file) {
    throw std::runtime_error{"file \"" + file_path +
                             "\" could not be opened for "
                             "reading"};
  }

  std::vector<std::string> file_contents{};

  while (input_file) {
    std::string new_line{};
    std::getline(input_file, new_line);

    if (insert_newlines == true) {
      new_line.push_back(m_k_newline);
    }

    file_contents.push_back(new_line);
  }

  return file_contents;
}

libconfigfile::file_pos::file_pos(const file *file_in_which_to_move)
    : m_file{file_in_which_to_move}, m_line{0}, m_char{0}, m_bof{false},
      m_eof{false} {
  if (file_in_which_to_move == nullptr) {
    throw std::runtime_error{"file cannot be null"};
  }
}

libconfigfile::file_pos::file_pos(const file *file_in_which_to_move,
                                  const file_pos &start_pos)
    : m_file{file_in_which_to_move}, m_line{start_pos.m_line},
      m_char{start_pos.m_char}, m_bof{start_pos.m_bof}, m_eof{start_pos.m_eof} {
  if (file_in_which_to_move == nullptr) {
    throw std::runtime_error{"file cannot be null"};
  }
}

libconfigfile::file_pos::file_pos(const file_pos &other)
    : m_file{other.m_file}, m_line{other.m_line}, m_char{other.m_char},
      m_bof{other.m_bof}, m_eof{other.m_eof} {}

libconfigfile::file_pos::file_pos(file_pos &&other)
    : m_file{std::move(other.m_file)}, m_line{std::move(other.m_line)},
      m_char{std::move(other.m_char)}, m_bof{std::move(other.m_bof)},
      m_eof{std::move(other.m_eof)} {}

libconfigfile::file_pos::~file_pos() {}

bool libconfigfile::file_pos::is_paired(const file &f) const {
  return m_file == &f;
}

const libconfigfile::file *libconfigfile::file_pos::get_paired_file() const {
  return m_file;
}

bool libconfigfile::file_pos::is_bof() const { return m_bof; }

bool libconfigfile::file_pos::is_eof() const { return m_eof; }

size_t libconfigfile::file_pos::get_line() const { return m_line; }

size_t libconfigfile::file_pos::get_char() const { return m_char; }

void libconfigfile::file_pos::set_line(const size_t line_val) {
  if (line_val >= m_file->get_array().size()) {
    throw std::runtime_error{"line value is out of range"};
  } else {
    m_line = line_val;
  }
}

void libconfigfile::file_pos::set_char(const size_t char_val) {
  if (char_val >= m_file->get_line(*this).size()) {
    throw std::runtime_error{"char value is out of range"};
  } else {
    m_char = char_val;
  }
}

void libconfigfile::file_pos::goto_next_line(size_t lines_to_move /*= 1*/) {
  if (lines_to_move == 0) {
    return;
  } else {
    if (m_eof == true) {
      return;
    } else if ((m_line + lines_to_move) < (m_file->get_array().size())) {
      m_line += lines_to_move;
      m_char = 0;
      m_bof = false;
    } else {
      set_eof();
      m_bof = false;
    }
  }
}

void libconfigfile::file_pos::goto_prev_line(size_t lines_to_move /*= 1*/) {
  if (lines_to_move == 0) {
    return;
  } else {
    if (m_bof == true) {
      return;
    } else if ((lines_to_move) <= (m_line)) {
      m_line -= lines_to_move;
      m_char = 0;
      m_eof = false;
    } else {
      set_bof();
      m_eof = false;
    }
  }
}

void libconfigfile::file_pos::goto_next_char(size_t chars_to_move /*= 1*/) {
  while (true) {
    if (m_eof == true) {
      return;
    } else if ((m_char + chars_to_move) < (m_file->get_line(*this).size())) {
      m_char += chars_to_move;
      chars_to_move = 0;
      m_bof = false;
      return;
    } else {
      chars_to_move -= (m_file->get_line(*this).size() - m_char);
      goto_next_line();
    }
  }
}

void libconfigfile::file_pos::goto_prev_char(size_t chars_to_move /*= 1*/) {
  while (true) {
    if (m_bof == true) {
      return;
    } else if ((chars_to_move) <= (m_char)) {
      m_char -= chars_to_move;
      chars_to_move = 0;
      m_eof = false;
      return;
    } else {
      size_t temp_char{m_char};
      goto_prev_line();
      m_char -= (m_file->get_line(*this).size() + temp_char);
    }
  }
}

void libconfigfile::file_pos::goto_find_start(const std::string &to_find) {
  if (m_eof == true) {
    return;
  }

  while (true) {
    std::string::size_type pos{m_file->get_line(*this).find(to_find, m_char)};

    if (pos == std::string::npos) {
      goto_next_line();

      if (m_eof == true) {
        return;
      } else {
        continue;
      }
    } else {
      m_char = pos;
      return;
    }
  }
}

void libconfigfile::file_pos::goto_find_end(const std::string &to_find) {
  if (m_eof == true) {
    return;
  }

  goto_find_start(to_find);
  goto_next_char(to_find.size());
}

void libconfigfile::file_pos::goto_rfind_start(const std::string &to_find) {
  if (m_bof == true) {
    return;
  }

  while (true) {
    std::string::size_type pos{m_file->get_line(*this).rfind(to_find, m_char)};

    if (pos == std::string::npos) {
      goto_prev_line();

      if (m_bof == true) {
        return;
      } else {
        continue;
      }
    } else {
      m_char = pos;
      return;
    }
  }
}

void libconfigfile::file_pos::goto_rfind_end(const std::string &to_find) {
  if (m_bof == true) {
    return;
  }

  goto_rfind_start(to_find);
  goto_next_char(to_find.size());
}

void libconfigfile::file_pos::goto_find_first_of(const std::string &to_find) {
  if (m_eof == true) {
    return;
  }

  while (true) {
    std::string::size_type pos{
        m_file->get_line(*this).find_first_of(to_find, m_char)};

    if (pos == std::string::npos) {
      goto_next_line();

      if (m_eof == true) {
        return;
      } else {
        continue;
      }
    } else {
      m_char = pos;
      return;
    }
  }
}

void libconfigfile::file_pos::goto_find_first_of(
    const std::vector<char> &to_find) {
  goto_find_first_of(std::string{to_find.begin(), to_find.end()});
}

void libconfigfile::file_pos::goto_find_first_not_of(
    const std::string &to_find) {
  if (m_eof == true) {
    return;
  }

  while (true) {
    std::string::size_type pos{
        m_file->get_line(*this).find_first_not_of(to_find, m_char)};

    if (pos == std::string::npos) {
      goto_next_line();

      if (m_eof == true) {
        return;
      } else {
        continue;
      }
    } else {
      m_char = pos;
      return;
    }
  }
}

void libconfigfile::file_pos::goto_find_first_not_of(
    const std::vector<char> &to_find) {
  goto_find_first_not_of(std::string{to_find.begin(), to_find.end()});
}

void libconfigfile::file_pos::goto_find_last_of(const std::string &to_find) {
  if (m_bof == true) {
    return;
  }

  while (true) {
    std::string::size_type pos{
        m_file->get_line(*this).find_last_of(to_find, m_char)};

    if (pos == std::string::npos) {
      goto_prev_line();

      if (m_bof == true) {
        return;
      } else {
        continue;
      }
    } else {
      m_char = pos;
      return;
    }
  }
}

void libconfigfile::file_pos::goto_find_last_of(
    const std::vector<char> &to_find) {
  goto_find_last_of(std::string{to_find.begin(), to_find.end()});
}

void libconfigfile::file_pos::goto_find_last_not_of(
    const std::string &to_find) {
  if (m_bof == true) {
    return;
  }

  const std::string to_find_str{to_find.begin(), to_find.end()};

  while (true) {
    std::string::size_type pos{
        m_file->get_line(*this).find_last_not_of(to_find_str, m_char)};

    if (pos == std::string::npos) {
      goto_prev_line();

      if (m_bof == true) {
        return;
      } else {
        continue;
      }
    } else {
      m_char = pos;
      return;
    }
  }
}

void libconfigfile::file_pos::goto_find_last_not_of(
    const std::vector<char> &to_find) {
  goto_find_last_not_of(std::string{to_find.begin(), to_find.end()});
}

void libconfigfile::file_pos::goto_end_of_whitespace(
    const std::string &whitespace_chars /*= " \t"*/) {
  if (m_eof == true) {
    return;
  }

  static const auto is_whitespace{[&whitespace_chars](char c) -> bool {
    for (size_t i{0}; i < whitespace_chars.size(); ++i) {
      if (c == whitespace_chars[i]) {
        return true;
      }
    }

    return false;
  }};

  if (is_whitespace(m_file->get_char(*this)) == false) {
    return;
  }

  while (true) {
    std::string::size_type pos{
        m_file->get_line(*this).find_first_not_of(whitespace_chars, m_char)};

    if (pos == std::string::npos) {
      goto_next_line();

      if (m_eof == true) {
        return;
      } else {
        continue;
      }
    } else {
      m_char = pos;
      return;
    }
  }
}

void libconfigfile::file_pos::goto_end_of_whitespace(
    const std::vector<char> &whitespace_chars) {
  goto_end_of_whitespace(
      std::string{whitespace_chars.begin(), whitespace_chars.end()});
}

void libconfigfile::file_pos::goto_start_of_whitespace(
    const std::string &whitespace_chars /*= " \t"*/) {
  if (m_bof == true) {
    return;
  }

  static const auto is_whitespace{[&whitespace_chars](char c) -> bool {
    for (size_t i{0}; i < whitespace_chars.size(); ++i) {
      if (c == whitespace_chars[i]) {
        return true;
      }
    }

    return false;
  }};

  if (is_whitespace(m_file->get_char(*this)) == false) {
    return;
  }

  while (true) {
    std::string::size_type pos{
        m_file->get_line(*this).find_last_not_of(whitespace_chars, m_char)};

    if (pos == std::string::npos) {
      goto_prev_line();

      if (m_bof == true) {
        return;
      } else {
        continue;
      }
    } else {
      m_char = pos;
      return;
    }
  }
}

void libconfigfile::file_pos::goto_start_of_whitespace(
    const std::vector<char> &whitespace_chars) {
  goto_start_of_whitespace(
      std::string{whitespace_chars.begin(), whitespace_chars.end()});
}

bool libconfigfile::file_pos::is_located_on_occurence_of(
    const std::string &str) const {
  return ((m_char) == ((m_file->get_line(*this)).find(str, m_char)));
}

bool libconfigfile::file_pos::is_located_on_occurence_of(const char ch) const {
  return ((m_file->get_char(*this)) == (ch));
}

libconfigfile::file_pos &
libconfigfile::file_pos::operator=(const file_pos &other) {
  m_file = other.m_file;
  m_line = other.m_line;
  m_char = other.m_char;
  m_bof = other.m_bof;
  m_eof = other.m_eof;

  return *this;
}

libconfigfile::file_pos &libconfigfile::file_pos::operator=(file_pos &&other) {
  m_file = std::move(other.m_file);
  m_line = std::move(other.m_line);
  m_char = std::move(other.m_char);
  m_bof = std::move(other.m_bof);
  m_eof = std::move(other.m_eof);

  return *this;
}

bool libconfigfile::file_pos::operator==(const file_pos &other) const {
  return ((m_line == other.m_line) && (m_char == other.m_char));
}

bool libconfigfile::file_pos::operator!=(const file_pos &other) const {
  return (!(operator==(other)));
}

libconfigfile::file_pos &libconfigfile::file_pos::operator++() {
  goto_next_char();
  return *this;
}

libconfigfile::file_pos libconfigfile::file_pos::operator++(int) {
  file_pos temp{*this};
  operator++();
  return temp;
}

libconfigfile::file_pos &libconfigfile::file_pos::operator--() {
  goto_prev_char();
  return *this;
}

libconfigfile::file_pos libconfigfile::file_pos::operator--(int) {
  file_pos temp{*this};
  operator--();
  return temp;
}

libconfigfile::file_pos &
libconfigfile::file_pos::operator+=(const size_t chars_to_move) {
  goto_next_char(chars_to_move);
  return *this;
}

libconfigfile::file_pos &
libconfigfile::file_pos::operator-=(const size_t chars_to_move) {
  goto_prev_char(chars_to_move);
  return *this;
}

void libconfigfile::file_pos::set_bof() {
  m_bof = true;
  m_line = 0;
  m_char = 0;
}

void libconfigfile::file_pos::set_eof() {
  m_eof = true;
  m_line = (m_file->get_array().size() - 1);
  m_char = (m_file->get_line(*this).size() - 1);
}

libconfigfile::file_pos libconfigfile::operator+(file_pos pos,
                                                 const size_t chars_to_move) {
  pos.goto_next_char(chars_to_move);
  return pos;
}

libconfigfile::file_pos libconfigfile::operator+(const size_t chars_to_move,
                                                 file_pos pos) {
  pos.goto_next_char(chars_to_move);
  return pos;
}

libconfigfile::file_pos libconfigfile::operator-(file_pos pos,
                                                 const size_t chars_to_move) {
  pos.goto_prev_char(chars_to_move);
  return pos;
}

libconfigfile::file_pos libconfigfile::operator-(const size_t chars_to_move,
                                                 file_pos pos) {
  pos.goto_prev_char(chars_to_move);
  return pos;
}
