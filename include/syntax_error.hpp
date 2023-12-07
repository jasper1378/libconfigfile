#ifndef LIBCONFIGFILE_SYNTAX_ERROR_HPP
#define LIBCONFIGFILE_SYNTAX_ERROR_HPP

#include <exception>
#include <stdexcept>
#include <string>

namespace libconfigfile {
class syntax_error : public std::runtime_error {
private:
  using base_t = std::runtime_error;

private:
  static constexpr char m_k_separator_char{':'};
  static constexpr char m_k_whitespace_char{' '};

private:
  std::string m_message;
  std::string m_category;
  std::string m_file_path;
  long long m_pos_line;
  long long m_pos_char;

public:
  explicit syntax_error(const std::string &message, const std::string &category,
                        const std::string &file_path, const long long pos_line,
                        const long long pos_char);

  syntax_error(const syntax_error &other);

  virtual ~syntax_error() override;

public:
  syntax_error &operator=(const syntax_error &other);

public:
  virtual const char *what() const noexcept override;
  std::string message() const;
  std::string category() const;
  std::string file_path() const;
  long long pos_line() const;
  long long pos_char() const;
};
} // namespace libconfigfile

#endif
