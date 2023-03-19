#ifndef LIBCONFIGFILE_PARSER_HPP
#define LIBCONFIGFILE_PARSER_HPP

#include "array_value_node.hpp"
#include "end_value_node.hpp"
#include "file.hpp"
#include "node.hpp"
#include "node_ptr.hpp"
#include "section_node.hpp"
#include "value_node.hpp"

#include <algorithm>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>
#include <vector>

#include <string_view>

namespace libconfigfile {
class parser {
private:
  file m_file_contents;
  file_pos m_cur_pos;
  section_node m_root_section;

private:
  static constexpr std::string m_k_whitespace_chars{" \t"};
  static constexpr char m_k_newline{'\n'};

  static constexpr char m_k_comment_script{'#'};
  static constexpr std::string m_k_comment_cpp{"//"};
  static constexpr std::string m_k_comment_c_start{"/*"};
  static constexpr std::string m_k_comment_c_end{"*/"};

  static constexpr char m_k_directive_leader{'@'};
  static constexpr std::string m_k_version_directive_name{"version"};
  static constexpr std::string m_k_include_directive_name{"include"};
  static constexpr int m_k_max_directive_name_length{std::max(
      m_k_version_directive_name.size(), m_k_include_directive_name.size())};

  static constexpr char m_k_string_delimiter{'"'};

  static constexpr char m_k_escape_leader{'\\'};
  static const std::unordered_map<char, char> m_k_basic_escape_chars;
  static constexpr char m_k_hex_escape_char{'x'};
  static constexpr int m_k_ascii_start{0x00};
  static constexpr int m_k_ascii_end{0x7F};

  static const std::string m_k_valid_name_chars;

  static constexpr char m_k_section_name_opening_delimiter{'('};
  static constexpr char m_k_section_name_closing_delimiter{')'};
  static constexpr char m_k_section_body_opening_delimiter{'{'};
  static constexpr char m_k_section_body_closing_delimiter{'}'};

  static constexpr char m_k_key_value_assign{'='};
  static constexpr char m_k_key_value_terminate{';'};

  static constexpr char m_k_int_digit_separator{'_'};
  static constexpr char m_k_int_positive_sign{'+'};
  static constexpr char m_k_int_negative_sign{'-'};

  struct numeral_system {
    int base;
    char prefix;
    char prefix_alt;
    std::string digits;
  };

  static constexpr numeral_system m_k_dec_num_sys{10, '\0', '\0', "0123456789"};
  static constexpr numeral_system m_k_bin_num_sys{2, 'b', 'B', "01"};
  static constexpr numeral_system m_k_oct_num_sys{8, 'o', 'O', "01234567"};
  static const numeral_system m_k_hex_num_sys;

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
  void parse_file(); // TODO

  std::tuple<node_ptr<section_node>, std::string>
  parse_section(bool is_root_section = false); // TODO

  std::tuple<node_ptr<value_node>, std::string> parse_key_value(); // TODO
  std::string parse_key_value_key();
  node_ptr<value_node> parse_key_value_value();   // TODO
  node_ptr<array_value_node> parse_array_value(); // TODO
  node_ptr<end_value_node<integer_end_value_node_t>>
  parse_integer_value(const std::string &raw_value); // TODO
  node_ptr<end_value_node<float_end_value_node_t>>
  parse_float_value(const std::string &raw_value); // TODO
  node_ptr<end_value_node<string_end_value_node_t>>
  parse_string_value(const std::string &raw_value); // TODO

  void parse_directive();
  void parse_version_directive();
  void parse_include_directive();

  void handle_comments();

private:
  static std::variant<std::string /*result*/,
                      std::string::size_type /*invalid_escape_sequence_pos*/>
  replace_escape_sequences(const std::string &str);

  static std::variant<std::vector<std::vector<std::string>> /*result*/,
                      std::string::size_type /*unterminated_string_pos*/>
  extract_strings(const std::string &raw,
                  const char delimiter = m_k_string_delimiter,
                  const char delimiter_escape = m_k_escape_leader,
                  const std::string &whitespace_chars = m_k_whitespace_chars);

  static std::string
  get_substr_between_indices_inclusive(const std::string &str,
                                       const std::string::size_type start,
                                       const std::string::size_type end);
  static std::string
  get_substr_between_indices_exclusive(const std ::string &str,
                                       const std::string::size_type start,
                                       const std::string::size_type end);
  static bool
  is_whitespace(const char ch,
                const std::string &whitespace_chars = m_k_whitespace_chars);
  static std::string
  trim_whitespace(const std::string &str,
                  const std::string &whitespace_chars = m_k_whitespace_chars,
                  bool trim_leading = true, bool trim_trailing = true);
  static bool
  is_actual_delimiter(const std::string::size_type pos, const std::string &str,
                      const char delimiter,
                      const char delimiter_escape = m_k_escape_leader);
  static bool
  is_invalid_character_valid_provided(const char ch,
                                      const std::string &valid_chars);
  static bool
  is_invalid_character_invalid_provided(const char ch,
                                        const std::string &invalid_chars);
  static std::tuple<bool, std::string::size_type>
  contains_invalid_character_valid_provided(const std::string &str,
                                            const std::string &valid_chars);
  static std::tuple<bool, std::string::size_type>
  contains_invalid_character_invalid_provided(const std::string &str,
                                              const std::string &invalid_chars);

  static bool is_digit(char ch,
                       const numeral_system &num_sys = m_k_dec_num_sys);
};
} // namespace libconfigfile

#endif
