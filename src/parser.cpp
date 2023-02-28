#include "parser.hpp"

#include "array_value_node.hpp"
#include "end_value_node.hpp"
#include "file.hpp"
#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"
#include "section_node.hpp"
#include "semantic_error.hpp"
#include "syntax_error.hpp"
#include "value_node.hpp"

#include <cstddef>
#include <exception>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

const std::unordered_map<char, char>
    libconfigfile::parser::m_k_basic_escape_chars{
        {'a', 0x07}, {'b', 0x08}, {'f', 0x0C},  {'n', 0x0A},  {'r', 0x0D},
        {'t', 0x09}, {'v', 0x0B}, {'\\', 0x5C}, {'\'', 0x27}, {'"', 0x22}};

const std::string libconfigfile::parser::m_k_hex_digits{
    "0123456789abcdefABCDEF"};

const std::string libconfigfile::parser::m_k_valid_name_chars{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"};

libconfigfile::parser::parser()
    : m_file_contents{}, m_cur_pos{m_file_contents.create_file_pos()},
      m_root_section{} {}

libconfigfile::parser::parser(const std::string &file_name)
    : m_file_contents{file_name}, m_cur_pos{m_file_contents.create_file_pos()},
      m_root_section{} {}

libconfigfile::parser::parser(const parser &other)
    : m_file_contents{other.m_file_contents},
      m_cur_pos{m_file_contents.create_file_pos(other.m_cur_pos)},
      m_root_section{other.m_root_section} {}

libconfigfile::parser::parser(parser &&other)
    : m_file_contents{std::move(other.m_file_contents)},
      m_cur_pos{m_file_contents.create_file_pos(other.m_cur_pos)},
      m_root_section{std::move(other.m_root_section)} {}

libconfigfile::parser::~parser() {}

libconfigfile::parser &libconfigfile::parser::operator=(const parser &other) {
  m_file_contents = other.m_file_contents;
  m_cur_pos = m_file_contents.create_file_pos(other.m_cur_pos);
  m_root_section = other.m_root_section;

  return *this;
}

libconfigfile::parser &libconfigfile::parser::operator=(parser &&other) {
  m_file_contents = std::move(other.m_file_contents);
  m_cur_pos = m_file_contents.create_file_pos(other.m_cur_pos);
  m_root_section = std::move(other.m_root_section);

  return *this;
}

// TODO move stuff into parse_section()
void libconfigfile::parser::parse_file() {
  while (m_cur_pos.is_eof() == false) {
    m_cur_pos.goto_end_of_whitespace(m_k_whitespace_chars);

    if (m_cur_pos.is_located_on_occurence_of(m_k_comment_script)) {
      m_cur_pos.goto_next_line();
    } else if (m_cur_pos.is_located_on_occurence_of(m_k_comment_cpp)) {
      m_cur_pos.goto_next_line();
    } else if (m_cur_pos.is_located_on_occurence_of(m_k_comment_c_start)) {
      m_cur_pos.goto_find_end(m_k_comment_c_end);
    }

    else if (m_cur_pos.is_located_on_occurence_of(m_k_directive_leader)) {
      parse_directive();
    }
  }
  // TODO
}

std::tuple<libconfigfile::node_ptr<libconfigfile::section_node>, std::string>
libconfigfile::parser::parse_section(bool is_root_section /*= false*/) {
  // if is_root_section is true, m_cur_pos is located at file start, else
  // m_cur_pos is located on opening round bracket

  std::string section_name{};

  if (is_root_section == false) {
    if (m_cur_pos.is_located_on_occurence_of(
            m_k_section_name_opening_delimiter) == false) {
      throw std::runtime_error{"parser::parse_section() called with m_cur_pos "
                               "in incorrect position"};
    }

    ++m_cur_pos;
    if (m_cur_pos.is_eof() == true) {
      std::string what_arg{"expected section name"};
      throw syntax_error::generate_formatted_error(
          m_file_contents, m_cur_pos.get_end_of_file_pos(), what_arg);
    }
    m_cur_pos.goto_end_of_whitespace(m_k_whitespace_chars);
    if (m_cur_pos.is_eof()) {
      std::string what_arg{"expected section name"};
      throw syntax_error::generate_formatted_error(
          m_file_contents, m_cur_pos.get_end_of_file_pos(), what_arg);
    }
    file_pos section_name_start_pos{m_cur_pos};

    m_cur_pos.goto_find_start(m_k_section_name_closing_delimiter);
    if (m_cur_pos.is_eof() == true) {
      std::string what_arg{"unterminated section name"};
      throw syntax_error::generate_formatted_error(
          m_file_contents, m_cur_pos.get_end_of_file_pos(), what_arg);
    }

    file_pos section_name_closing_delimiter{m_cur_pos};
    --m_cur_pos;
    m_cur_pos.goto_start_of_whitespace(m_k_whitespace_chars);
    file_pos section_name_end_pos{m_cur_pos};

    if (section_name_start_pos.get_line() != section_name_end_pos.get_line()) {
      std::string what_arg{"section name must appear completely on one line"};
      throw syntax_error::generate_formatted_error(
          m_file_contents, section_name_end_pos, what_arg);
    }

    std::string section_name_raw{get_substr_between_indices_inclusive(
        m_file_contents.get_line(section_name_start_pos),
        section_name_start_pos.get_char(), section_name_end_pos.get_char())};
    section_name_raw =
        trim_whitespace(section_name_raw, m_k_whitespace_chars, true, true);

    if (section_name_raw.empty() == true) {
      std::string what_arg{"empty section names are not permitted"};
      throw syntax_error::generate_formatted_error(
          m_file_contents, section_name_end_pos, what_arg);
    }

    std::tuple<bool, std::string::size_type> section_name_raw_invalid_chars{
        contains_invalid_character_valid_provided(section_name_raw,
                                                  m_k_valid_name_chars)};

    if (std::get<0>(section_name_raw_invalid_chars) == true) {
      std::string what_arg{"invalid character in section name"};
      throw syntax_error::generate_formatted_error(
          m_file_contents, section_name_end_pos.get_line(),
          std::get<1>(section_name_raw_invalid_chars), what_arg);
    }

    section_name = section_name_raw;
    m_cur_pos = section_name_closing_delimiter;
  } else {
    section_name = "";
  }

  // TODO
}

void libconfigfile::parser::parse_directive() {
  // m_cur_pos = directive leader

  file_pos test_pos{m_cur_pos - 1};
  test_pos.goto_find_last_not_of(m_k_whitespace_chars);
  if ((test_pos.get_line() == m_cur_pos.get_line()) &&
      (test_pos.is_bof() == false)) {
    std::string what_arg{"directive must be the only text on its line"};
    throw syntax_error::generate_formatted_error(m_file_contents, m_cur_pos,
                                                 what_arg);
  } else {

    const std::string &directive_line{m_file_contents.get_line(m_cur_pos)};
    std::string::size_type line_pos{m_cur_pos.get_char()};

    ++line_pos;
    line_pos = directive_line.find_first_not_of(m_k_whitespace_chars, line_pos);
    if (line_pos == std::string::npos) {
      std::string what_arg{"expected directive name"};
      throw syntax_error::generate_formatted_error(
          m_file_contents, m_cur_pos.get_line(), line_pos, what_arg);
    } else {
      std::string::size_type start_of_name{line_pos};

      line_pos = directive_line.find_first_of(m_k_whitespace_chars, line_pos);
      if (line_pos == std::string::npos) {
        line_pos = (directive_line.size() - 1);
      }
      std::string::size_type end_of_name{line_pos};

      std::string name{get_substr_between_indices_inclusive(
          directive_line, start_of_name, end_of_name)};

      std::string args{};
      std::string::size_type start_of_args{};
      ++line_pos;
      line_pos =
          directive_line.find_first_not_of(m_k_whitespace_chars, line_pos);
      if (line_pos == std::string::npos) {
        start_of_args = end_of_name;
        args = "";
      } else {
        start_of_args = line_pos;
        args = get_substr_between_indices_inclusive(
            directive_line, start_of_args, (directive_line.size() - 1));
      }

      m_cur_pos.set_char(start_of_args);

      if (name == "version") {
        std::string what_arg{
            "alpha version does not support version directive"};
        throw syntax_error::generate_formatted_error(
            m_file_contents, m_cur_pos.get_line(), start_of_name, what_arg);
      } else if (name == "include") {
        parse_include_directive(args);
      } else {
        std::string what_arg{"unknown directive"};
        throw syntax_error::generate_formatted_error(
            m_file_contents, m_cur_pos.get_line(), start_of_name, what_arg);
      }
    }
  }
}

void libconfigfile::parser::parse_directive_new() {
  // m_cur_pos = directive leader
  // caller must check that directive is the only text on its line

  file_pos start_pos{m_cur_pos};

  std::string name{};
  name.reserve(m_k_max_directive_name_length);

  {
    enum class name_location {
      directive_leader,
      leading_whitespace,
      name_proper,
      done,
    };

    name_location last_state{name_location::directive_leader};
    file_pos last_pos{m_cur_pos};

    for (;; last_pos = m_cur_pos, ++m_cur_pos) {
      switch (last_state) {
      case name_location::directive_leader: {
        if (m_cur_pos.is_eof() == true) {
          std::string what_arg{"expected directive name"};
          throw syntax_error::generate_formatted_error(
              m_file_contents, m_cur_pos.get_end_of_file_pos(), what_arg);
        } else {
          char cur_char{m_file_contents.get_char(m_cur_pos)};

          if (cur_char == m_k_directive_leader) {
            continue;
          } else if (is_whitespace(cur_char) == true) {
            last_state = name_location::leading_whitespace;
            continue;
          } else {
            if (m_cur_pos.get_line() != start_pos.get_line()) {
              std::string what_arg{"entire directive must appear on one line"};
              throw syntax_error::generate_formatted_error(m_file_contents,
                                                           m_cur_pos, what_arg);
            } else {
              last_state = name_location::name_proper;
              name.push_back(cur_char);
            }
          }
        }
      } break;

      case name_location::leading_whitespace: {
        if (m_cur_pos.is_eof() == true) {
          std::string what_arg{"expected directive name"};
          throw syntax_error::generate_formatted_error(
              m_file_contents, m_cur_pos.get_end_of_file_pos(), what_arg);
        } else {
          char cur_char{m_file_contents.get_char(m_cur_pos)};

          if (is_whitespace(cur_char) == true) {
            continue;
          } else {
            if (m_cur_pos.get_line() != start_pos.get_line()) {
              std::string what_arg{"entire directive must appear on one line"};
              throw syntax_error::generate_formatted_error(m_file_contents,
                                                           m_cur_pos, what_arg);
            } else {
              last_state = name_location::name_proper;
              name.push_back(cur_char);
            }
          }
        }
      } break;

      case name_location::name_proper: {
        if (m_cur_pos.is_eof() == true) {
          last_state = name_location::done;
          goto goto_exit_name_loop;
        } else {
          char cur_char{m_file_contents.get_char(m_cur_pos)};

          if (is_whitespace(cur_char) == true) {
            last_state = name_location::done;
            goto goto_exit_name_loop;
          } else {
            if (m_cur_pos.get_line() != start_pos.get_line()) {
              std::string what_arg{"entire directive must appear on one line"};
              throw syntax_error::generate_formatted_error(m_file_contents,
                                                           m_cur_pos, what_arg);
            } else {
              name.push_back(cur_char);
            }
          }
        }
      } break;

      case name_location::done: {
        goto goto_exit_name_loop;
      } break;
      }
    }
  goto_exit_name_loop:;
  }

  void (parser::*directive_function)(){nullptr};

  if (name == m_k_version_directive_name) {
    directive_function = &parser::parse_version_directive;
  } else if (name == m_k_include_directive_name) {
    directive_function = &parser::parse_include_directive_new;
  } else {
    std::string what_arg{"invalid directive"};
    throw syntax_error::generate_formatted_error(m_file_contents, m_cur_pos,
                                                 what_arg);
  }

  for (;; ++m_cur_pos) {
    if (m_cur_pos.is_eof() == true) {
      break;
    } else {
      char cur_char{m_file_contents.get_char(m_cur_pos)};

      if (is_whitespace(cur_char) == true) {
        continue;
      } else {
        break;
      }
    }
  }

  (this->*directive_function)();
}

void libconfigfile::parser::parse_version_directive() {
  std::string what_arg{"alpha version does not support version directive"};
  throw syntax_error::generate_formatted_error(m_file_contents, m_cur_pos,
                                               what_arg);
}

void libconfigfile::parser::parse_include_directive(const std::string &args) {
  // m_cur_pos =
  // start of directive arguments if arguments exist or
  // end of directive name if arguments don't exist

  if (args.empty() == true) {
    std::string what_arg{"include directive requires file path argument"};
    throw syntax_error::generate_formatted_error(m_file_contents, m_cur_pos,
                                                 what_arg);
  } else if (args.front() != m_k_string_delimiter) {
    std::string what_arg{"include directive requires file path argument"};
    throw syntax_error::generate_formatted_error(m_file_contents, m_cur_pos,
                                                 what_arg);
  } else {
    std::string file_path{};
    file_path.reserve(args.size());

    std::string::size_type end_pos{std::string::npos};

    for (std::string::size_type i{1}; i < args.size(); ++i) {
      if (is_actual_delimiter(i, args, m_k_string_delimiter,
                              m_k_escape_leader)) {
        end_pos = i;
      } else {
        file_path.push_back(args[i]);
      }
    }

    if (end_pos == std::string::npos) {
      std::string what_arg{"unterminated string in include directive argument"};
      throw syntax_error::generate_formatted_error(
          m_file_contents, m_cur_pos.get_line(),
          (m_file_contents.get_line(m_cur_pos).size() - 1), what_arg);
    }

    {
      std::string::size_type extra_args_pos{
          args.find_first_not_of(m_k_whitespace_chars, (end_pos + 1))};
      if (extra_args_pos != std::string::npos) {
        std::string what_arg{"excess arguments given to include directive"};
        throw syntax_error::generate_formatted_error(
            m_file_contents, m_cur_pos.get_line(),
            (m_cur_pos.get_char() + extra_args_pos), what_arg);
      }
    }

    std::variant<std::string, std::string::size_type> file_path_escaped{
        replace_escape_sequences(file_path)};

    switch (file_path_escaped.index()) {
    case 0: {
      file_path = std::get<std::string>(std::move(file_path_escaped));

      file included_file{file_path};

      std::remove_reference_t<decltype(m_file_contents.get_underlying())>::
          const_iterator include_pos_iter{
              m_file_contents.get_underlying().begin() + m_cur_pos.get_line()};

      m_file_contents.get_underlying().erase(include_pos_iter);
      m_file_contents.get_underlying().insert(
          include_pos_iter,
          std::make_move_iterator(included_file.get_underlying().begin()),
          std::make_move_iterator(included_file.get_underlying().end()));

      m_cur_pos = m_file_contents.create_file_pos(m_cur_pos);
      m_cur_pos.set_char(0);
      return;
    } break;

    case 1: {
      std::string::size_type invalid_escape_sequence_pos{
          std::get<std::string::size_type>(file_path_escaped)};

      std::string what_arg{
          "invalid escape sequence in include directive argument"};
      throw syntax_error::generate_formatted_error(
          m_file_contents, m_cur_pos.get_line(),
          (m_cur_pos.get_char() + invalid_escape_sequence_pos), what_arg);
    } break;
    }
  }
}

void libconfigfile::parser::parse_include_directive_new() {
  // m_cur_pos = start of directive arguments
  // or eof if arguments don't exist
  // TODO continue from here
}

std::variant<std::string /*result*/,
             std::string::size_type /*invalid_escape_sequence_pos*/>
libconfigfile::parser::replace_escape_sequences(const std::string &str) {
  static const auto is_hex_digit{[](const char ch) {
    return ((m_k_hex_digits.find(ch)) != (std::string::npos));
  }};

  std::string result{};
  result.reserve(str.size());

  for (std::string::size_type cur_char{0}; cur_char < str.size(); ++cur_char) {
    if (str[cur_char] == m_k_escape_leader) {
      std::string::size_type escape_char_pos{cur_char + 1};
      if (escape_char_pos < str.size()) {
        char escape_char{str[escape_char_pos]};
        if (escape_char == m_k_hex_escape_char) {
          std::string::size_type hex_digit_pos_1{escape_char_pos + 1};
          std::string::size_type hex_digit_pos_2{escape_char_pos + 2};
          if ((hex_digit_pos_1 < str.size()) &&
              (hex_digit_pos_2 < str.size())) {
            char hex_digit_1{str[hex_digit_pos_1]};
            char hex_digit_2{str[hex_digit_pos_2]};
            if ((is_hex_digit(hex_digit_1)) && (is_hex_digit(hex_digit_2))) {
              std::string hex_string{std::string{} + hex_digit_1 + hex_digit_2};
              result.push_back(
                  static_cast<char>(std::stoi(hex_string, nullptr, 16)));
              cur_char = hex_digit_pos_2;
            } else {
              return cur_char;
            }
          } else {
            return cur_char;
          }
        } else {
          if (m_k_basic_escape_chars.contains(escape_char)) {
            result.push_back(m_k_basic_escape_chars.at(escape_char));
            cur_char = escape_char;
          } else {
            return cur_char;
          }
        }
      } else {
        return cur_char;
      }
    } else {
      result.push_back(str[cur_char]);
    }
  }
  return result;
}

std::variant<std::vector<std::vector<std::string>> /*result*/,
             std::string::size_type /*unterminated_string_pos*/>
libconfigfile::parser::extract_strings(
    const std::string &raw, const char delimiter /*= m_k_string_delimiter*/,
    const char delimiter_escape /*= m_k_escape_leader*/,
    const std::string &whitespace_chars /*= m_k_whitespace_chars*/) {

  bool in_string{false};
  bool strings_are_adjacent{true};

  std::string::size_type start_of_last_string{std::string::npos};

  std::string cur_string{};
  std::vector<std::string> cur_string_group{};
  std::vector<std::vector<std::string>> all_strings{};

  for (std::string::size_type cur_char{0}; cur_char < raw.size(); ++cur_char) {
    if (in_string == true) {
      if (is_actual_delimiter(cur_char, raw, delimiter, delimiter_escape) ==
          true) {
        in_string = false;
        strings_are_adjacent = true;
        if (cur_string.empty() == false) {
          cur_string_group.push_back(std::move(cur_string));
          cur_string.clear();
        }
      } else {
        cur_string.push_back(raw[cur_char]);
      }
    } else {
      if (is_actual_delimiter(cur_char, raw, delimiter, delimiter_escape) ==
          true) {
        in_string = true;
        start_of_last_string = cur_char;
        if (strings_are_adjacent == false) {
          if (cur_string_group.empty() == false) {
            all_strings.push_back(std::move(cur_string_group));
            cur_string_group.clear();
          }
        } else {
          if (is_whitespace(raw[cur_char], whitespace_chars) == false) {
            strings_are_adjacent = false;
          }
        }
      }
    }
  }
  all_strings.push_back(std::move(cur_string_group));
  cur_string_group.clear();

  if (in_string == true) {
    return start_of_last_string;
  } else {
    return all_strings;
  }
}

std::string libconfigfile::parser::get_substr_between_indices_inclusive(
    const std::string &str, const std::string::size_type start,
    const std::string::size_type end) {
  if (start > end) {
    return "";
  } else if (start >= str.size()) {
    return "";
  } else {
    return str.substr((start), ((end - start) + 1));
  }
}

std::string libconfigfile::parser::get_substr_between_indices_exclusive(
    const std::string &str, const std::string::size_type start,
    const std::string::size_type end) {

  if (start > end) {
    return "";
  } else if (start >= str.size()) {
    return "";
  } else if ((end - start) <= 1) {
    return "";
  } else {
    return str.substr((start + 1), (((end - 1) - (start + 1)) + 1));
  }
}

bool libconfigfile::parser::is_whitespace(
    const char ch,
    const std::string &whitespace_chars /*= m_k_whitespace_chars*/) {
  return ((whitespace_chars.find(ch)) != (std::string::npos));
}

std::string libconfigfile::parser::trim_whitespace(
    const std::string &str,
    const std::string &whitespace_chars /*= m_k_whitespace_chars*/,
    bool trim_leading /*= true*/, bool trim_trailing /*= true*/) {
  if (str.empty() == true) {
    return "";
  } else {
    std::string::size_type start_pos{0};
    start_pos = str.find_first_not_of(whitespace_chars);

    std::string::size_type end_pos{str.size() - 1};
    end_pos = str.find_last_not_of(whitespace_chars);

    if ((trim_leading == false) && (trim_trailing == false)) {
      return str;
    } else if ((trim_leading == true) && (trim_trailing == false)) {
      return get_substr_between_indices_inclusive(str, start_pos,
                                                  std::string::npos);
    } else if ((trim_leading == false) && (trim_trailing == true)) {
      return get_substr_between_indices_inclusive(str, 0, end_pos);
    } else if ((trim_leading == true) && (trim_trailing == true)) {
      return get_substr_between_indices_inclusive(str, start_pos, end_pos);
    } else {
      throw std::runtime_error{"impossible!"};
    }
  }
}

bool libconfigfile::parser::is_actual_delimiter(
    const std::string::size_type pos, const std::string &str,
    const char delimiter,
    const char delimiter_escape /*= m_k_delimiter_leader*/) {
  if (pos < str.size()) {
    if (str[pos] == delimiter) {
      if (pos == 0) {
        return true;
      } else if (str[pos - 1] == delimiter_escape) {
        return false;
      } else {
        return true;
      }
    } else {
      return false;
    }
  } else {
    return false;
  }
}

bool libconfigfile::parser::is_invalid_character_valid_provided(
    const char ch, const std::string &valid_chars) {
  return (valid_chars.find(ch) == std::string::npos);
}

bool libconfigfile::parser::is_invalid_character_invalid_provided(
    const char ch, const std::string &invalid_chars) {
  return (invalid_chars.find(ch) != std::string::npos);
}

std::tuple<bool, std::string::size_type>
libconfigfile::parser::contains_invalid_character_valid_provided(
    const std::string &str, const std::string &valid_chars) {
  for (size_t i{0}; i < str.size(); ++i) {
    if (is_invalid_character_valid_provided(str[i], valid_chars) == true) {
      return {true, i};
    }
  }
  return {false, std::string::npos};
}

std::tuple<bool, std::string::size_type>
libconfigfile::parser::contains_invalid_character_invalid_provided(
    const std::string &str, const std::string &invalid_chars) {
  for (size_t i{0}; i < str.size(); ++i) {
    if (is_invalid_character_invalid_provided(str[i], invalid_chars) == true) {
      return {true, i};
    }
  }
  return {false, std::string::npos};
}
