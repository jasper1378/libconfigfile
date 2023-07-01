#ifndef LIBCONFIGFILE_SYNTAX_ERROR_HPP
#define LIBCONFIGFILE_SYNTAX_ERROR_HPP

#include "file.hpp"

#include <exception>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace libconfigfile {
class syntax_error : public std::runtime_error {
public:
  explicit syntax_error(const std::string &what_arg);
  explicit syntax_error(const char *what_arg);
  syntax_error(const syntax_error &other) noexcept;
  syntax_error(syntax_error &&other) noexcept;

  virtual ~syntax_error() override;

public:
  virtual const char *what() const noexcept override;

public:
  syntax_error &operator=(const syntax_error &other) noexcept;
  syntax_error &operator=(syntax_error &&other) noexcept;

public:
  static syntax_error
  generate_formatted_error(const std::string &what_arg,
                           const std::filesystem::path &file_path,
                           const size_t pos_line, const size_t pos_char);
  static syntax_error generate_formatted_error(const std::string &what_arg,
                                               const std::string &file_path,
                                               const size_t pos_line,
                                               const size_t pos_char);
};
} // namespace libconfigfile

#endif
