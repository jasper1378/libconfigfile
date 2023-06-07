#ifndef LIBCONFIGFILE_PARSER_HPP
#define LIBCONFIGFILE_PARSER_HPP

#include "array_value_node.hpp"
#include "character_constants.hpp"
#include "end_value_node.hpp"
#include "float_end_value_node.hpp"
#include "integer_end_value_node.hpp"
#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"
#include "numeral_system.hpp"
#include "section_node.hpp"
#include "string_end_value_node.hpp"
#include "value_node.hpp"

#include <algorithm>
#include <cmath>
#include <concepts>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace libconfigfile {
namespace parser {
node_ptr<section_node> parse(const std::filesystem::path &file_path);

namespace impl {
struct context {
  std::filesystem::path file_path;
  std::ifstream file;
  node_ptr<section_node> root_section;
};

enum class directive {
  null,
  version,
  include,
};

node_ptr<section_node> parse(const std::filesystem::path &file_path);

std::pair<std::string, node_ptr<section_node>>
parse_section(context &ctx, bool is_root_section = false);

std::pair<std::string, node_ptr<value_node>> parse_key_value(context &ctx);
std::string parse_key_value_key(context &ctx);
node_ptr<value_node> parse_key_value_value(context &ctx);

node_ptr<array_value_node>
parse_array_value(context &ctx, const std::string &raw_value,
                  const std::ifstream::pos_type &start_pos);
node_ptr<integer_end_value_node>
parse_integer_value(context &ctx, const std::string &raw_value,
                    const std::ifstream::pos_type &start_pos);
node_ptr<float_end_value_node>
parse_float_value(context &ctx, const std::string &raw_value,
                  const std::ifstream::pos_type &start_pos);
node_ptr<string_end_value_node>
parse_string_value(context &ctx, const std::string &raw_value,
                   const std::ifstream::pos_type &start_pos);

node_ptr<value_node>
call_appropriate_value_parse_func(context &ctx, const std::string &raw_value,
                                  const std::ifstream::pos_type &start_pos);

std::pair<directive, std::optional<node_ptr<section_node>>>
parse_directive(context &ctx);
void parse_version_directive(context &ctx);
node_ptr<section_node> parse_include_directive(context &ctx);

bool handle_comments(context &ctx);

std::variant<value_node_type, end_value_node_type>
identify_key_value_value_type(const std::string &value_contents);
end_value_node_type
identify_key_value_numeric_value_type(const std::string &value_contents);

std::variant<std::string /*result*/,
             std::string::size_type /*invalid_escape_sequence_pos*/>
replace_escape_sequences(const std::string &str);

std::variant<std::vector<std::vector<std::string>> /*result*/,
             std::string::size_type /*unterminated_string_pos*/>
extract_strings(
    const std::string &raw,
    const char delimiter = character_constants::g_k_string_delimiter,
    const char delimiter_escape = character_constants::g_k_escape_leader,
    const std::string &whitespace_chars =
        character_constants::g_k_whitespace_chars);

std::string
get_substr_between_indices_inclusive(const std::string &str,
                                     const std::string::size_type start,
                                     const std::string::size_type end);
std::string
get_substr_between_indices_exclusive(const std ::string &str,
                                     const std::string::size_type start,
                                     const std::string::size_type end);
bool is_whitespace(const char ch,
                   const std::string &whitespace_chars =
                       character_constants::g_k_whitespace_chars);
std::string trim_whitespace(const std::string &str,
                            const std::string &whitespace_chars =
                                character_constants::g_k_whitespace_chars,
                            bool trim_leading = true,
                            bool trim_trailing = true);
bool is_actual_delimiter(
    const std::string::size_type pos, const std::string &str,
    const char delimiter,
    const char delimiter_escape = character_constants::g_k_escape_leader);
bool is_invalid_character_valid_provided(const char ch,
                                         const std::string &valid_chars);
bool is_invalid_character_invalid_provided(const char ch,
                                           const std::string &invalid_chars);
std::pair<bool, std::string::size_type>
contains_invalid_character_valid_provided(const std::string &str,
                                          const std::string &valid_chars);
std::pair<bool, std::string::size_type>
contains_invalid_character_invalid_provided(const std::string &str,
                                            const std::string &invalid_chars);

bool is_digit(char ch, const numeral_system &num_sys = numeral_system_decimal);

bool case_insensitive_string_compare(const std::string &str1,
                                     const std::string &str2);
std::string::size_type case_insensitive_string_find(const std::string &str,
                                                    const std::string &to_find);

template <std::floating_point T>
bool compare_floats(T a, T b, T abs_epsilon = T{1e-12},
                    T rel_epsilon = T{1e-8}) {
  T diff{std::abs(a - b)};
  if (diff <= abs_epsilon) {
    return true;
  } else {
    return (diff <= (std::max(std::abs(a), std::abs(b)) * rel_epsilon));
  }
}

bool string_contains_only(const std::string &str, const std::string &chars);
bool string_contains_any_of(const std::string &str, const std::string &chars);

std::string string_to_upper(const std::string &str);
std::string string_to_lower(const std::string &str);
} // namespace impl
} // namespace parser
} // namespace libconfigfile

#endif
