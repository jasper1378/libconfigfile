#include "parser.hpp"

#include "array_value_node.hpp"
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
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

const std::unordered_map<std::string, char>
    libconfigfile::parser::m_k_basic_escape_sequences{
        {"\a", 0x07}, {"\b", 0x08}, {"\f", 0x0C}, {"\n", 0x0A}, {"\r", 0x0D},
        {"\t", 0x09}, {"\v", 0x0B}, {"\\", 0x5C}, {"\'", 0x27}, {"\"", 0x22}};

libconfigfile::parser::parser()
    : m_file_contents{}, m_cur_pos{m_file_contents.create_file_pos()} {}

libconfigfile::parser::parser(const std::string &file_name)
    : m_file_contents{file_name}, m_cur_pos{m_file_contents.create_file_pos()} {
}

libconfigfile::parser::parser(const parser &other)
    : m_file_contents{other.m_file_contents},
      m_cur_pos{m_file_contents.create_file_pos(other.m_cur_pos)} {}

libconfigfile::parser::parser(parser &&other)
    : m_file_contents{std::move(other.m_file_contents)},
      m_cur_pos{m_file_contents.create_file_pos(other.m_cur_pos)} {}

libconfigfile::parser::~parser() {}

libconfigfile::parser &libconfigfile::parser::operator=(const parser &other) {
  m_file_contents = other.m_file_contents;
  m_cur_pos = m_file_contents.create_file_pos(other.m_cur_pos);

  return *this;
}

libconfigfile::parser &libconfigfile::parser::operator=(parser &&other) {
  m_file_contents = std::move(other.m_file_contents);
  m_cur_pos = m_file_contents.create_file_pos(other.m_cur_pos);

  return *this;
}

void libconfigfile::parser::parse_file() {
  static const auto is_whitespace{[](char c) {
    for (size_t i{0}; i < m_k_whitespace_chars.size(); ++i) {
      if (c == m_k_whitespace_chars[i]) {
        return true;
      }
    }

    return false;
  }};

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
      file_pos test_pos{m_cur_pos - 1};
      test_pos.goto_find_last_not_of(m_k_whitespace_chars);
      if ((test_pos.get_line() == m_cur_pos.get_line()) &&
          (test_pos.is_bof() == false)) {
        std::string what_arg{"directive must be the only text on its line"};
        throw syntax_error::generate_formatted_error(m_file_contents, m_cur_pos,
                                                     what_arg);
      } else {
        parse_directive();
        // TODO continue
        // from here
      }
    }
  }
}

void libconfigfile::parser::parse_directive() {
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

    std::string name{
        get_substr_between_indices(directive_line, start_of_name, end_of_name)};

    std::string args{};
    std::string::size_type start_of_args{};
    ++line_pos;
    line_pos = directive_line.find_first_not_of(m_k_whitespace_chars, line_pos);
    if (line_pos == std::string::npos) {
      args = "";
    } else {
      start_of_args = line_pos;
      args = get_substr_between_indices(directive_line, start_of_args,
                                        (directive_line.size() - 1));
    }

    m_cur_pos.set_char(start_of_args);

    if (name == "version") {
      std::string what_arg{"alpha version does not support version directive"};
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

void libconfigfile::parser::parse_include_directive(const std::string &args) {
  if (args.empty() == true) {
    std::string what_arg{"include directive requires file path argument"};
    throw syntax_error::generate_formatted_error(m_file_contents, m_cur_pos,
                                                 what_arg);
  } else if (args.front() != '"') {
    std::string what_arg{"include directive requires file path argument"};
    throw syntax_error::generate_formatted_error(m_file_contents, m_cur_pos,
                                                 what_arg);
  }
  // TODO continue from here
}

std::variant<std::vector<std::vector<std::string>> /*result*/,
             std::string::size_type /*unterminated_string_pos*/>
libconfigfile::parser::extract_strings(
    const std::string &raw, const char delimiter /*= '"'*/,
    const char delimiter_escape /*= '\\'*/,
    const std::string &whitespace_chars /*= m_k_whitespace_chars*/) {

  bool in_string{false};
  bool strings_are_adjacent{true};

  std::string::size_type start_of_last_string{std::string::npos};
  std::string::size_type end_of_last_string{std::string::npos};

  std::string cur_string{};
  std::vector<std::string> cur_string_group{};
  std::vector<std::vector<std::string>> all_strings{};

  static const auto is_whitespace{[&whitespace_chars](const char ch) -> bool {
    return ((whitespace_chars.find(ch)) != (std::string::npos));
  }};

  static const auto is_actual_delimiter{
      [delimiter, delimiter_escape, &raw](std::string::size_type pos) -> bool {
        if ((pos >= 0) && (pos < raw.size())) {
          if (raw[pos] == delimiter) {
            if (pos == 0) {
              return true;
            } else if (raw[pos - 1] == delimiter_escape) {
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
      }};

  for (std::string::size_type cur_char{0}; cur_char < raw.size(); ++cur_char) {
    if (in_string == true) {
      if (is_actual_delimiter(cur_char) == true) {
        in_string = false;
        end_of_last_string = cur_char;
        strings_are_adjacent = true;
        if (cur_string.empty() == false) {
          cur_string_group.push_back(std::move(cur_string));
          cur_string.clear();
        }
      } else {
        cur_string.push_back(raw[cur_char]);
      }
    } else {
      if (is_actual_delimiter(cur_char) == true) {
        in_string = true;
        start_of_last_string = cur_char;
        if (strings_are_adjacent == false) {
          if (cur_string_group.empty() == false) {
            all_strings.push_back(std::move(cur_string_group));
            cur_string_group.clear();
          }
        } else {
          if (is_whitespace(raw[cur_char]) == false) {
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

char libconfigfile::parser::escape_sequence_to_char( // XXX
    const std::string &seq, bool throw_on_invalid_seq /*= false*/) {
  static constexpr int basic_esc_seq_len{2};
  static constexpr int hexdec_esc_seq_len{4};

  static const auto return_invalid_seq{[throw_on_invalid_seq, &seq]() {
    if (throw_on_invalid_seq == true) {
      throw std::runtime_error{"invalid escape sequence: \"" + seq + "\""};
    } else {
      return 0x00;
    }
  }};

  if (seq.front() != '\\') {
    return_invalid_seq();
  } else {
    if (seq.size() == basic_esc_seq_len) {
      if (m_k_basic_escape_sequences.contains(seq)) {
        return m_k_basic_escape_sequences.at(seq);
      } else {
        return_invalid_seq();
      }
    } else if (seq.size() == hexdec_esc_seq_len) {
      if (seq[1] != 'x') {
        return_invalid_seq();
      } else {
        return static_cast<char>(
            std::stoi(get_substr_between_indices(seq, 2, 3), nullptr, 16));
      }
    } else {
      return_invalid_seq();
    }
  }
}

std::variant<std::string /*result*/,
             std::string::size_type /*invalid_escape_sequence_pos*/>
libconfigfile::parser::replace_escape_sequences(const std::string &str) {
} // TODO

std::string libconfigfile::parser::get_substr_between_indices(
    const std::string &str, const std::string::size_type start,
    const std::string::size_type end) {
  return str.substr((start), ((end - start) + 1));
}
