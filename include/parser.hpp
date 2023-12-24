#ifndef LIBCONFIGFILE_PARSER_HPP
#define LIBCONFIGFILE_PARSER_HPP

#include "array_node.hpp"
#include "character_constants.hpp"
#include "float_node.hpp"
#include "integer_node.hpp"
#include "map_node.hpp"
#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"
#include "numeral_system.hpp"
#include "string_node.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <istream>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace libconfigfile {
namespace parser {
node_ptr<map_node> parse(const std::string &identifier,
                         std::istream &input_stream,
                         const bool identifier_is_file_path = false);
node_ptr<map_node> parse_file(const char *file_path);
node_ptr<map_node> parse_file(const std::string &file_path);
node_ptr<map_node> parse_file(const std::filesystem::path &file_path);

namespace impl {

struct context {
  std::string identifier;
  std::istream &input_stream;
  bool identifier_is_file_path;
  long long line_count;
  long long char_count;
};

enum class directive {
  null,
  version,
  include,
};

node_ptr<map_node> parse(const std::string &identifier,
                         std::istream &input_stream,
                         const bool identifier_is_file_path = false);

std::pair<std::string, node_ptr<node>>
parse_key_value(context &ctx, const std::string &possible_terminating_chars,
                char *actual_terminating_char = nullptr);
std::string parse_key_value_key(context &ctx);
node_ptr<node>
parse_key_value_value(context &ctx,
                      const std::string &possible_terminating_chars,
                      char *actual_terminating_char = nullptr);

node_ptr<string_node>
parse_string_value(context &ctx, const std::string &possible_terminating_chars,
                   char *actual_terminating_char = nullptr);
node_ptr<integer_node>
parse_integer_value(context &ctx, const std::string &possible_terminating_chars,
                    char *actual_terminating_char = nullptr);
node_ptr<float_node>
parse_float_value(context &ctx, const std::string &possible_terminating_chars,
                  char *actual_terminating_char = nullptr);
node_ptr<array_node>
parse_array_value(context &ctx, const std::string &possible_terminating_chars,
                  char *actual_terminating_char = nullptr);
node_ptr<map_node>
parse_map_value(context &ctx, const std::string &possible_terminating_chars,
                char *actual_terminating_char = nullptr,
                const bool is_root_map = false);

node_ptr<node>
call_appropriate_value_parse_func(context &ctx,
                                  const std::string &possible_terminating_chars,
                                  char *actual_terminating_char = nullptr);

std::pair<directive, std::optional<node_ptr<map_node>>>
parse_directive(context &ctx);
void parse_version_directive(context &ctx);
node_ptr<map_node> parse_include_directive(context &ctx);

bool handle_comments(context &ctx);
char handle_escape_sequence(context &ctx);

node_type
identify_key_value_value_type(context &ctx,
                              const std::string &possible_terminating_chars,
                              char *actual_terminating_char = nullptr);
node_type identify_key_value_numeric_value_type(
    context &ctx, const std::string &possible_terminating_chars,
    char *actual_terminating_char = nullptr);

std::variant<std::string /*result*/,
             std::string::size_type /*invalid_escape_sequence_pos*/>
replace_escape_sequences(const std::string &str);

bool is_whitespace(const char ch,
                   const std::string &whitespace_chars =
                       character_constants::g_k_whitespace_chars);
bool is_invalid_character_valid_provided(const char ch,
                                         const std::string &valid_chars);
bool is_invalid_character_invalid_provided(const char ch,
                                           const std::string &invalid_chars);

bool is_digit(const char ch,
              const numeral_system &num_sys = numeral_system_decimal);

bool case_insensitive_char_compare(const char ch1, const char ch2);
bool case_insensitive_string_compare(const std::string &str1,
                                     const std::string &str2);
std::string::size_type case_insensitive_string_find(const std::string &str,
                                                    const std::string &to_find);
} // namespace impl
} // namespace parser
using parser::parse;
using parser::parse_file;
} // namespace libconfigfile

#endif
