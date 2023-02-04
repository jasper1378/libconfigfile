#include "parser.hpp"

#include "array_value_node.hpp"
#include "config.hpp"
#include "end_value_node.hpp"
#include "file.hpp"
#include "float_end_value_node.hpp"
#include "integer_end_value_node.hpp"
#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"
#include "section_node.hpp"
#include "semantic_error.hpp"
#include "string_end_value_node.hpp"
#include "syntax_error.hpp"
#include "value_node.hpp"

#include <cstddef>
#include <exception>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

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
  if (this == &other) {
    return *this;
  }

  m_file_contents = other.m_file_contents;
  m_cur_pos = m_file_contents.create_file_pos(other.m_cur_pos);

  return *this;
}

libconfigfile::parser &libconfigfile::parser::operator=(parser &&other) {
  if (this == &other) {
    return *this;
  }

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

    if (m_file_contents[m_cur_pos] == m_k_comment_script) {
      m_cur_pos.goto_next_line();
    } else if (is_pos_located_on_occurence_of(m_cur_pos,
                                              m_k_comment_cpp)) // TODO c/c++
                                                                // comments
    {
      m_cur_pos.goto_next_line();
    } else if (is_pos_located_on_occurence_of(m_cur_pos, m_k_comment_c_start)) {
      m_cur_pos.goto_find_end(m_k_comment_c_end);
    } else if (m_file_contents[m_cur_pos] == m_k_directive_leader) {
      file_pos test_pos{m_cur_pos - 1};
      test_pos.goto_find_last_not_of(m_k_whitespace_chars);
      if ((test_pos.get_line() == m_cur_pos.get_line()) &&
          (test_pos.is_bof() == false)) {
        std::string what_arg{"directive must be the only text on its line"};
        throw syntax_error::generate_formatted_error(m_file_contents, m_cur_pos,
                                                     what_arg);
      }

      parse_directive();
      // TODO continue
      // from here
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
  }
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
    // TODO
  } else if (name == "include") {
    parse_include_directive(args);
  } else {
    std::string what_arg{"unknown directive"};
    throw syntax_error::generate_formatted_error(
        m_file_contents, m_cur_pos.get_line(), start_of_name, what_arg);
  }
}

void libconfigfile::parser::parse_include_directive(const std::string &args) {
  if (args[0] != '"') {
    std::string what_arg{"include directive requires file "
                         "path argument"};
  }
  // TODO continue from here
}

bool libconfigfile::parser::is_pos_located_on_occurence_of(
    const file_pos &pos, const std::string &str) {
  return ((pos.get_char()) ==
          (m_file_contents.get_line(pos).find(str, pos.get_char())));
}

std::string libconfigfile::parser::get_substr_between_indices(
    const std::string &str, const std::string::size_type start,
    const std::string::size_type end) {
  return str.substr((start), ((end - start) + 1));
}
