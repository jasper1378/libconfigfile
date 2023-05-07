#ifndef LIBCONFIGFILE_PARSER_HPP
#define LIBCONFIGFILE_PARSER_HPP

#include "array_value_node.hpp"
#include "character_constants.hpp"
#include "end_value_node.hpp"
#include "file.hpp"
#include "float_end_value_node.hpp"
#include "integer_end_value_node.hpp"
#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"
#include "section_node.hpp"
#include "string_end_value_node.hpp"
#include "value_node.hpp"

#include <algorithm>
#include <cmath>
#include <concepts>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace libconfigfile {
class parser {
private:
  file m_file_contents;
  file_pos m_cur_pos;
  node_ptr<section_node> m_root_section;

public:
  parser() = delete;
  parser(const std::string &file_name);
  parser(const parser &other) = delete;
  parser(parser &&other) = delete;

  ~parser();

public:
  node_ptr<section_node> get_result() const;

public:
  parser &operator=(const parser &other) = delete;
  parser &operator=(parser &&other) = delete;

private:
  std::pair<std::string, node_ptr<section_node>>
  parse_section(bool is_root_section = false);

  std::pair<std::string, node_ptr<value_node>> parse_key_value();
  std::string parse_key_value_key();
  node_ptr<value_node> parse_key_value_value();

  node_ptr<array_value_node> parse_array_value(const std::string &raw_value,
                                               const file_pos &start_pos);
  node_ptr<integer_end_value_node>
  parse_integer_value(const std::string &raw_value, const file_pos &start_pos);
  node_ptr<float_end_value_node> parse_float_value(const std::string &raw_value,
                                                   const file_pos &start_pos);
  node_ptr<string_end_value_node>
  parse_string_value(const std::string &raw_value, const file_pos &start_pos);

  node_ptr<value_node>
  call_appropriate_value_parse_func(const std::string &raw_value,
                                    const file_pos &start_pos);

  void parse_directive();
  void parse_version_directive();
  void parse_include_directive();

  void handle_comments();

private:
  static std::variant<value_node_type, end_value_node_type>
  identify_key_value_value_type(const std::string &value_contents);
  static end_value_node_type
  identify_key_value_numeric_value_type(const std::string &value_contents);

  static std::variant<std::string /*result*/,
                      std::string::size_type /*invalid_escape_sequence_pos*/>
  replace_escape_sequences(const std::string &str);

  static std::variant<std::vector<std::vector<std::string>> /*result*/,
                      std::string::size_type /*unterminated_string_pos*/>
  extract_strings(
      const std::string &raw,
      const char delimiter = character_constants::g_k_string_delimiter,
      const char delimiter_escape = character_constants::g_k_escape_leader,
      const std::string &whitespace_chars =
          character_constants::g_k_whitespace_chars);

  static std::string
  get_substr_between_indices_inclusive(const std::string &str,
                                       const std::string::size_type start,
                                       const std::string::size_type end);
  static std::string
  get_substr_between_indices_exclusive(const std ::string &str,
                                       const std::string::size_type start,
                                       const std::string::size_type end);
  static bool is_whitespace(const char ch,
                            const std::string &whitespace_chars =
                                character_constants::g_k_whitespace_chars);
  static std::string
  trim_whitespace(const std::string &str,
                  const std::string &whitespace_chars =
                      character_constants::g_k_whitespace_chars,
                  bool trim_leading = true, bool trim_trailing = true);
  static bool is_actual_delimiter(
      const std::string::size_type pos, const std::string &str,
      const char delimiter,
      const char delimiter_escape = character_constants::g_k_escape_leader);
  static bool
  is_invalid_character_valid_provided(const char ch,
                                      const std::string &valid_chars);
  static bool
  is_invalid_character_invalid_provided(const char ch,
                                        const std::string &invalid_chars);
  static std::pair<bool, std::string::size_type>
  contains_invalid_character_valid_provided(const std::string &str,
                                            const std::string &valid_chars);
  static std::pair<bool, std::string::size_type>
  contains_invalid_character_invalid_provided(const std::string &str,
                                              const std::string &invalid_chars);

  static bool is_digit(char ch, const numeral_system &num_sys =
                                    character_constants::g_k_dec_num_sys);

  static bool case_insensitive_string_compare(const std::string &str1,
                                              const std::string &str2);
  static std::string::size_type
  case_insensitive_string_find(const std::string &str,
                               const std::string &to_find);

  template <std::floating_point T>
  static bool compare_floats(T a, T b, T abs_epsilon = T{1e-12},
                             T rel_epsilon = T{1e-8}) {
    T diff{std::abs(a - b)};
    if (diff <= abs_epsilon) {
      return true;
    } else {
      return (diff <= (std::max(std::abs(a), std::abs(b)) * rel_epsilon));
    }
  }

  static bool string_contains_only(const std::string &str,
                                   const std::string &chars);
  static bool string_contains_any_of(const std::string &str,
                                     const std::string &chars);

  static std::string string_to_upper(const std::string &str);
  static std::string string_to_lower(const std::string &str);
};
} // namespace libconfigfile
#endif
