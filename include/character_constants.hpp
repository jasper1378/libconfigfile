#ifndef LIBCONFIGFILE_CHARACTER_CONSTANTS_HPP
#define LIBCONFIGFILE_CHARACTER_CONSTANTS_HPP

#include "float_end_value_node.hpp"
#include "numeral_system.hpp"

#include <string>
#include <unordered_map>

namespace libconfigfile {
namespace character_constants {
static constexpr char g_k_newline{'\n'};
static constexpr std::string g_k_whitespace_chars{' ', '\t'};

static constexpr char g_k_comment_script{'#'};
static constexpr std::string g_k_comment_cpp{"//"};
static constexpr std::string g_k_comment_c_start{"/*"};
static constexpr std::string g_k_comment_c_end{"*/"};

static constexpr char g_k_directive_leader{'@'};
static constexpr std::string g_k_version_directive_name{"version"};
static constexpr std::string g_k_include_directive_name{"include"};
static constexpr int g_k_max_directive_name_length{std::max(
    g_k_version_directive_name.size(), g_k_include_directive_name.size())};

static constexpr char g_k_array_opening_delimiter{'['};
static constexpr char g_k_array_closing_delimiter{']'};
static constexpr char g_k_array_element_separator{','};

static constexpr char g_k_string_delimiter{'"'};

static constexpr char g_k_escape_leader{'\\'};
static const std::unordered_map<char, char> g_k_basic_escape_chars{
    {'a', 0x07}, {'b', 0x08}, {'f', 0x0C},  {'n', 0x0A},  {'r', 0x0D},
    {'t', 0x09}, {'v', 0x0B}, {'\\', 0x5C}, {'\'', 0x27}, {'"', 0x22}};
static constexpr char g_k_hex_escape_char{'x'};
static constexpr int g_k_ascii_start{0x00};
static constexpr int g_k_ascii_end{0x7F};

static const std::string g_k_valid_name_chars{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"};

static constexpr char g_k_section_name_opening_delimiter{'('};
static constexpr char g_k_section_name_closing_delimiter{')'};
static constexpr char g_k_section_body_opening_delimiter{'{'};
static constexpr char g_k_section_body_closing_delimiter{'}'};

static constexpr char g_k_key_value_assign{'='};
static constexpr char g_k_key_value_terminate{';'};

static constexpr char g_k_num_digit_separator{'_'};
static constexpr char g_k_num_positive_sign{'+'};
static constexpr char g_k_num_negative_sign{'-'};

static constexpr char g_k_num_sys_prefix_leader{'0'};

static constexpr char g_k_float_decimal_point{'.'};
static constexpr char g_k_float_exponent_sign_lower{'e'};
static constexpr char g_k_float_exponent_sign_upper{'E'};
static constexpr std::pair<float_end_value_node_data_t, std::string>
    g_k_float_infinity{
        std::numeric_limits<float_end_value_node_data_t>::infinity(), "inf"};
static constexpr std::pair<float_end_value_node_data_t, std::string>
    g_k_float_not_a_number{
        std::numeric_limits<float_end_value_node_data_t>::quiet_NaN(), "nan"};
} // namespace character_constants
} // namespace libconfigfile

#endif
