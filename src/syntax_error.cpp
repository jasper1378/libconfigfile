#include "syntax_error.hpp"

#include <exception>
#include <stdexcept>
#include <string>
#include <utility>

libconfigfile::syntax_error::syntax_error(const std::string &what_arg)
    : std::runtime_error{what_arg} {}

libconfigfile::syntax_error::syntax_error(const char *what_arg)
    : std::runtime_error{what_arg} {}

libconfigfile::syntax_error::syntax_error(const syntax_error &other) noexcept
    : std::runtime_error{other} {}

libconfigfile::syntax_error::syntax_error(syntax_error &&other) noexcept
    : std::runtime_error{std::move(other)} {}

libconfigfile::syntax_error::~syntax_error() {}

const char *libconfigfile::syntax_error::what() const noexcept {
  return std::runtime_error::what();
}

libconfigfile::syntax_error &
libconfigfile::syntax_error::operator=(const syntax_error &other) noexcept {
  if (this == &other) {
    return *this;
  }

  std::runtime_error::operator=(other);

  return *this;
}

libconfigfile::syntax_error &
libconfigfile::syntax_error::operator=(syntax_error &&other) noexcept {
  if (this == &other) {
    return *this;
  }

  std::runtime_error::operator=(std::move(other));

  return *this;
}

libconfigfile::syntax_error
libconfigfile::syntax_error::generate_formatted_error(
    const std::string &file_name, const size_t pos_line, const size_t pos_char,
    const std::string &what_arg) {
  return syntax_error{file_name + ":" + std::to_string(pos_line) + ":" +
                      std::to_string(pos_char) + ": " + what_arg};
}

libconfigfile::syntax_error
libconfigfile::syntax_error::generate_formatted_error(
    const file &f, const file_pos &pos, const std::string &what_arg) {
  return generate_formatted_error(f.get_file_path(), pos.get_line(),
                                  pos.get_char(), what_arg);
}

libconfigfile::syntax_error
libconfigfile::syntax_error::generate_formatted_error(
    const std::string &file_name, const file_pos &pos,
    const std::string &what_arg) {
  return generate_formatted_error(file_name, pos.get_line(), pos.get_char(),
                                  what_arg);
}

libconfigfile::syntax_error
libconfigfile::syntax_error::generate_formatted_error(
    const file &f, const size_t pos_line, const size_t pos_char,
    const std::string &what_arg) {
  return generate_formatted_error(f.get_file_path(), pos_line, pos_char,
                                  what_arg);
}
