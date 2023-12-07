#include "syntax_error.hpp"

#include <exception>
#include <stdexcept>
#include <string>
#include <utility>

libconfigfile::syntax_error::syntax_error(const std::string &message,
                                          const std::string &category,
                                          const std::string &file_path,
                                          const long long pos_line,
                                          const long long pos_char)
    : base_t{file_path + m_k_separator_char + std::to_string(pos_line) +
             m_k_separator_char + std::to_string(pos_char) +
             m_k_separator_char + m_k_whitespace_char + category +
             m_k_separator_char + m_k_whitespace_char + message},
      m_message{message}, m_category{category}, m_file_path{file_path},
      m_pos_line{pos_line}, m_pos_char{pos_char} {}

libconfigfile::syntax_error::syntax_error(const syntax_error &other)
    : std::runtime_error{other} {}

libconfigfile::syntax_error::~syntax_error() {}

libconfigfile::syntax_error &
libconfigfile::syntax_error::operator=(const syntax_error &other) {
  if (this != &other) {
    std::runtime_error::operator=(other);
    m_message = other.m_message;
    m_category = other.m_category;
    m_file_path = other.m_file_path;
    m_pos_line = other.m_pos_line;
    m_pos_char = other.m_pos_char;
  }
  return *this;
}

const char *libconfigfile::syntax_error::what() const noexcept {
  return std::runtime_error::what();
}

std::string libconfigfile::syntax_error::message() const { return m_message; }

std::string libconfigfile::syntax_error::category() const { return m_category; }

std::string libconfigfile::syntax_error::file_path() const {
  return m_file_path;
}

long long libconfigfile::syntax_error::pos_line() const { return m_pos_line; }

long long libconfigfile::syntax_error::pos_char() const { return m_pos_char; }
