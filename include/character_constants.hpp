#ifndef LIBCONFIGFILE_CHARACTER_CONSTANTS_HPP
#define LIBCONFIGFILE_CHARACTER_CONSTANTS_HPP

#include "float_node.hpp"
#include "numeral_system.hpp"

#include <string>
#include <unordered_map>

namespace libconfigfile {
namespace character_constants {
static constexpr char k_newline{'\n'};
static constexpr char k_space{' '};
static constexpr char k_tab{'\t'};
static constexpr std::string k_whitespace_chars{k_space, k_tab};

static constexpr std::string k_indent_str(2, k_space);

static constexpr char k_comment_script{'#'};
static constexpr std::string k_comment_cpp{"//"};
static constexpr std::string k_comment_c_start{"/*"};
static constexpr std::string k_comment_c_end{"*/"};

static constexpr char k_key_value_assign{'='};
static constexpr char k_key_value_terminate{';'};
static const std::string k_valid_name_chars{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-"};

static constexpr char k_directive_leader{'@'};
static constexpr std::string k_version_directive_name{"version"};
static constexpr std::string k_include_directive_name{"include"};
static constexpr int k_max_directive_name_length{std::max(
    k_version_directive_name.size(), k_include_directive_name.size())};

static constexpr char k_map_opening_delimiter{'{'};
static constexpr char k_map_closing_delimiter{'}'};

static constexpr char k_array_opening_delimiter{'['};
static constexpr char k_array_closing_delimiter{']'};
static constexpr char k_array_element_separator{','};

static constexpr char k_string_delimiter{'"'};

static constexpr char k_escape_leader{'\\'};
static const std::unordered_map<char, char> k_basic_escape_chars{
    {'"', 0x22}, {'\\', 0x5c}, {'/', 0x2f}, {'b', 0x08},
    {'f', 0x0c}, {'n', 0x0a},  {'r', 0x0d}, {'t', 0x09},
};
static constexpr char k_hex_escape_char{'x'};
static constexpr int k_ascii_start{0x00};
static constexpr int k_ascii_end{0x7F};

static const std::string k_control_chars{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
    0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
    0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
static const std::unordered_map<char, std::string> k_control_chars_codes{
    {0x00, "x00"}, {0x01, "x01"}, {0x02, "x02"}, {0x03, "x03"}, {0x04, "x04"},
    {0x05, "x05"}, {0x06, "x06"}, {0x07, "x07"}, {0x08, "b"},   {0x09, "t"},
    {0x0a, "n"},   {0x0b, "x0b"}, {0x0c, "f"},   {0x0d, "r"},   {0x0e, "x0e"},
    {0x0f, "x0f"}, {0x10, "x10"}, {0x11, "x11"}, {0x12, "x12"}, {0x13, "x13"},
    {0x14, "x14"}, {0x15, "x15"}, {0x16, "x16"}, {0x17, "x17"}, {0x18, "x18"},
    {0x19, "x19"}, {0x1a, "x1a"}, {0x1b, "x1b"}, {0x1c, "x1c"}, {0x1d, "x1d"},
    {0x1e, "x1e"}, {0x1f, "x1f"}};

static constexpr char k_num_digit_separator{'_'};
static constexpr char k_num_positive_sign{'+'};
static constexpr char k_num_negative_sign{'-'};

static constexpr char k_num_sys_prefix_leader{'0'};

static constexpr char k_float_decimal_point{'.'};
static constexpr char k_float_exponent_sign_lower{'e'};
static constexpr char k_float_exponent_sign_upper{'E'};
static constexpr std::pair<float_node::base_t, std::string> k_float_infinity{
    std::numeric_limits<float_node::base_t>::infinity(), "inf"};
static constexpr std::pair<float_node::base_t, std::string>
    k_float_not_a_number{std::numeric_limits<float_node::base_t>::quiet_NaN(),
                           "nan"};
} // namespace character_constants
} // namespace libconfigfile

#endif
