#ifndef LIBCONFIGFILE_PARSER_HPP
#define LIBCONFIGFILE_PARSER_HPP

#include "file.hpp"
#include "node.hpp"
#include "node_ptr.hpp"

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace libconfigfile {
class parser {
private:
  file m_file_contents;
  file_pos m_cur_pos;

private:
  static constexpr std::string m_k_whitespace_chars{" \t"};
  static constexpr char m_k_newline{'\n'};

  static constexpr char m_k_comment_script{'#'};
  static constexpr std::string m_k_comment_cpp{"//"};
  static constexpr std::string m_k_comment_c_start{"/*"};
  static constexpr std::string m_k_comment_c_end{"*/"};

  static constexpr char m_k_directive_leader{'@'};

  static const std::unordered_map<std::string, char> m_k_basic_escape_sequences;

public:
  parser();
  parser(const std::string &file_name);
  parser(const parser &other);
  parser(parser &&other) /*TODO: noexcept()*/;

  ~parser();

public:
  // section_node parse() or section_node get_result() or etc.; // TODO

public:
  parser &operator=(const parser &other);
  parser &operator=(parser &&other) /*TODO: noexcept()*/;

private:
  void parse_file();

  void parse_directive();
  void parse_include_directive(const std::string &args);

private:
  static std::variant<std::vector<std::vector<std::string>> /*result*/,
                      std::string::size_type /*unterminated_string_pos*/>
  extract_strings(const std::string &raw, const char delimiter = '"',
                  const char delimiter_escape = '\\',
                  const std::string &whitespace_chars = m_k_whitespace_chars);

  static std::variant<std::string /*result*/,
                      std::string::size_type /*invalid_escape_sequence_pos*/>
  replace_escape_sequences(const std::string &str);

  static std::string
  get_substr_between_indices(const std::string &str,
                             const std::string::size_type start,
                             const std::string::size_type end);
};
} // namespace libconfigfile

#endif
