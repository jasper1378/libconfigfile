#include "parser.hpp"

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
#include "semantic_error.hpp"
#include "string_end_value_node.hpp"
#include "syntax_error.hpp"
#include "value_node.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <exception>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

libconfigfile::parser::parser(const std::string &file_name)
    : m_file_contents{file_name}, m_cur_pos{m_file_contents.create_file_pos()},
      m_root_section{parse_section(true).second} {}

libconfigfile::parser::~parser() {}

libconfigfile::node_ptr<libconfigfile::section_node>
libconfigfile::parser::get_result() const {
  return m_root_section;
}

std::pair<std::string, libconfigfile::node_ptr<libconfigfile::section_node>>
libconfigfile::parser::parse_section(bool is_root_section) {
  // m_cur_pos = opening name delimiter
  //  or beginning of file is is_root_section is true

  std::pair<std::string, node_ptr<section_node>> ret_val{
      "", make_node_ptr<section_node>()};

  if (is_root_section == false) {

    enum class name_location {
      opening_delimiter,
      leading_whitespace,
      name_proper,
      trailing_whitespace,
      closing_delimiter,
      done,
    };

    file_pos start_of_name_proper_pos{m_cur_pos.get_end_of_file_pos()};

    bool first_loop{true};

    for (name_location last_state{name_location::opening_delimiter};
         last_state != name_location::done; ++m_cur_pos, first_loop = false) {

      switch (last_state) {
      case name_location::opening_delimiter: {
        if (m_cur_pos.is_eof() == true) {
          std::string what_arg{"expected section name"};
          throw syntax_error::generate_formatted_error(m_file_contents,
                                                       m_cur_pos, what_arg);
        } else {
          char cur_char{m_file_contents.get_char(m_cur_pos)};

          if (is_whitespace(cur_char) == true) {
            last_state = name_location::leading_whitespace;
          } else {
            if ((first_loop == true) &&
                (cur_char ==
                 character_constants::g_k_section_name_opening_delimiter)) {
              ;
            } else {
              if (cur_char ==
                  character_constants::g_k_section_name_closing_delimiter) {
                last_state = name_location::closing_delimiter;

                std::string what_arg{"empty section names are not permitted"};
                throw syntax_error::generate_formatted_error(
                    m_file_contents, m_cur_pos, what_arg);
              } else {
                last_state = name_location::name_proper;
                start_of_name_proper_pos = m_cur_pos;

                if (is_invalid_character_valid_provided(
                        cur_char, character_constants::g_k_valid_name_chars) ==
                    true) {
                  std::string what_arg{"invalid character in section name"};
                  throw syntax_error::generate_formatted_error(
                      m_file_contents, m_cur_pos, what_arg);
                } else {
                  ret_val.first.push_back(cur_char);
                }
              }
            }
          }
        }
      } break;

      case name_location::leading_whitespace: {
        if (m_cur_pos.is_eof() == true) {
          std::string what_arg{"expected section name"};
          throw syntax_error::generate_formatted_error(m_file_contents,
                                                       m_cur_pos, what_arg);
        } else {
          char cur_char{m_file_contents.get_char(m_cur_pos)};

          if (is_whitespace(cur_char) == true) {
            ;
          } else {
            if (cur_char ==
                character_constants::g_k_section_name_closing_delimiter) {
              last_state = name_location::closing_delimiter;

              std::string what_arg{"empty section names are not permitted"};
              throw syntax_error::generate_formatted_error(m_file_contents,
                                                           m_cur_pos, what_arg);
            } else {
              last_state = name_location::name_proper;
              start_of_name_proper_pos = m_cur_pos;

              if (is_invalid_character_valid_provided(
                      cur_char, character_constants::g_k_valid_name_chars) ==
                  true) {
                std::string what_arg{"invalid character in section name"};
                throw syntax_error::generate_formatted_error(
                    m_file_contents, m_cur_pos, what_arg);
              } else {
                ret_val.first.push_back(cur_char);
              }
            }
          }
        }
      } break;

      case name_location::name_proper: {
        if (m_cur_pos.is_eof() == true) {
          std::string what_arg{"unterminated section name"};
          throw syntax_error::generate_formatted_error(m_file_contents,
                                                       m_cur_pos, what_arg);
        } else {
          char cur_char{m_file_contents.get_char(m_cur_pos)};

          if (is_whitespace(cur_char) == true) {
            last_state = name_location::trailing_whitespace;
          } else {
            if (cur_char ==
                character_constants::g_k_section_name_closing_delimiter) {
              last_state = name_location::closing_delimiter;
            } else {
              if (m_cur_pos.get_line() != start_of_name_proper_pos.get_line()) {
                std::string what_arg{
                    "section name must appear completely on one line"};
                throw syntax_error::generate_formatted_error(
                    m_file_contents, m_cur_pos, what_arg);
              } else {
                if (is_invalid_character_valid_provided(
                        cur_char, character_constants::g_k_valid_name_chars) ==
                    true) {
                  std::string what_arg{"invalid character in section name"};
                  throw syntax_error::generate_formatted_error(
                      m_file_contents, m_cur_pos, what_arg);
                } else {
                  ret_val.first.push_back(cur_char);
                }
              }
            }
          }
        }
      } break;

      case name_location::trailing_whitespace: {
        if (m_cur_pos.is_eof() == true) {
          std::string what_arg{"unterminated section name"};
          throw syntax_error::generate_formatted_error(m_file_contents,
                                                       m_cur_pos, what_arg);
        } else {
          char cur_char{m_file_contents.get_char(m_cur_pos)};

          if (is_whitespace(cur_char) == true) {
            ;
          } else {
            if (cur_char ==
                character_constants::g_k_section_name_closing_delimiter) {
              last_state = name_location::closing_delimiter;
            } else {
              std::string what_arg{
                  "character after trailing whitespace in section name"};
              throw syntax_error::generate_formatted_error(m_file_contents,
                                                           m_cur_pos, what_arg);
            }
          }
        }
      } break;

      case name_location::closing_delimiter: {
        last_state = name_location::done;
      } break;

      case name_location::done: {
        throw std::runtime_error{"impossible!"};
      } break;
      }
    }
  } else {
    ret_val.first = "";
  }

  // m_cur_pos is one past name closing delimiter

  if (is_root_section == false) {

    enum class name_body_gap_location {
      separating_whitespace,
      opening_body_delimiter,
      done,
    };

    for (name_body_gap_location last_state{
             name_body_gap_location::separating_whitespace};
         last_state != name_body_gap_location::done; ++m_cur_pos) {
      switch (last_state) {

      case name_body_gap_location::separating_whitespace: {
        if (m_cur_pos.is_eof() == true) {
          std::string what_arg{"expected section body"};
          throw syntax_error::generate_formatted_error(m_file_contents,
                                                       m_cur_pos, what_arg);
        } else {
          char cur_char{m_file_contents.get_char(m_cur_pos)};

          if (is_whitespace(cur_char) == true) {
            ;
          } else if (cur_char ==
                     character_constants::g_k_section_body_opening_delimiter) {
            last_state = name_body_gap_location::opening_body_delimiter;
          } else {
            std::string what_arg{"expected section body opening delimiter"};
            throw syntax_error::generate_formatted_error(m_file_contents,
                                                         m_cur_pos, what_arg);
          }
        }
      } break;

      case name_body_gap_location::opening_body_delimiter: {
        last_state = name_body_gap_location::done;
      } break;

      case name_body_gap_location::done: {
        throw std::runtime_error{"impossible!"};
      } break;
      }
    }
  }

  // m_cur_pos is one past body opening delimiter

  {
    bool ended_on_body_closing_delimiter{false};
    for (;; ++m_cur_pos) {
      if (m_cur_pos.is_eof() == true) {
        break;
      } else {
        char cur_char{m_file_contents.get_char(m_cur_pos)};

        if (is_whitespace(cur_char)) {
          ;
        } else if (cur_char ==
                   character_constants::g_k_section_body_closing_delimiter) {
          ended_on_body_closing_delimiter = true;
          ++m_cur_pos;
          break;
        } else if (cur_char ==
                   character_constants::g_k_section_name_opening_delimiter) {
          file_pos start_pos{m_cur_pos};

          std::pair<std::string, node_ptr<section_node>> new_section{
              parse_section(false)};

          if (ret_val.second->contains(new_section.first) == true) {
            std::string what_arg{"duplicate name in scope"};
            throw syntax_error::generate_formatted_error(m_file_contents,
                                                         start_pos, what_arg);
          } else {
            ret_val.second->insert({std::move(new_section)});
          }
        } else if (false) {
          // TODO directives
        } else {
          file_pos start_pos{m_cur_pos};

          std::pair<std::string, node_ptr<value_node>> new_key_value{
              parse_key_value()};

          if (ret_val.second->contains(new_key_value.first) == true) {
            std::string what_arg{"duplicate name in scope"};
            throw syntax_error::generate_formatted_error(m_file_contents,
                                                         start_pos, what_arg);
          } else {
            ret_val.second->insert({std::move(new_key_value)});
          }
        }
      }
    }

    if ((ended_on_body_closing_delimiter == false) &&
        (is_root_section == false)) {
      std::string what_arg{"expected section body closing delimiter"};
      throw syntax_error::generate_formatted_error(m_file_contents, m_cur_pos,
                                                   what_arg);
    }
  }

  return ret_val;

  // m_cur_pos is one past closing body delimiter
}

std::pair<std::string, libconfigfile::node_ptr<libconfigfile::value_node>>
libconfigfile::parser::parse_key_value() {
  // m_cur_pos = first char of key name or leading whitespace before key name

  std::pair<std::string, node_ptr<value_node>> ret_val{};

  ret_val.first = parse_key_value_key();
  ret_val.second = parse_key_value_value();

  return ret_val;
}

std::string libconfigfile::parser::parse_key_value_key() {
  // m_cur_pos = first char of key name or leading whitespace before key name

  std::string key_name{};

  enum class key_name_location {
    leading_whitespace,
    name_proper,
    trailing_whitespace,
    equal_sign /*i.e. done*/,
  };

  for (key_name_location last_state{key_name_location::leading_whitespace};
       last_state != key_name_location::equal_sign; ++m_cur_pos) {

    switch (last_state) {

    case key_name_location::leading_whitespace: {
      if (m_cur_pos.is_eof() == true) {
        std::string what_arg{"expected key name"};
        throw syntax_error::generate_formatted_error(m_file_contents, m_cur_pos,
                                                     what_arg);
      } else {
        char cur_char{m_file_contents.get_char(m_cur_pos)};

        if (is_whitespace(cur_char)) {
          ;
        } else {
          last_state = key_name_location::name_proper;

          if (is_invalid_character_valid_provided(
                  cur_char, character_constants::g_k_valid_name_chars) ==
              true) {
            switch (cur_char) {

            case character_constants::g_k_key_value_assign: {
              std::string what_arg{"missing key-value name"};
              throw syntax_error::generate_formatted_error(m_file_contents,
                                                           m_cur_pos, what_arg);
            } break;

            case character_constants::g_k_key_value_terminate: {
              std::string what_arg{"empty key-value"};
              throw syntax_error::generate_formatted_error(m_file_contents,
                                                           m_cur_pos, what_arg);
            } break;

            default: {
              std::string what_arg{"invalid character in key name"};
              throw syntax_error::generate_formatted_error(m_file_contents,
                                                           m_cur_pos, what_arg);
            } break;
            }

          } else {
            key_name.push_back(cur_char);
          }
        }
      }
    } break;

    case key_name_location::name_proper: {
      if (m_cur_pos.is_eof() == true) {
        std::string what_arg{"missing value part of key-value"};
        throw syntax_error::generate_formatted_error(m_file_contents, m_cur_pos,
                                                     what_arg);
      } else {
        char cur_char{m_file_contents.get_char(m_cur_pos)};

        if (is_whitespace(cur_char) == true) {
          last_state = key_name_location::trailing_whitespace;
        } else {
          if (cur_char == character_constants::g_k_key_value_assign) {
            last_state = key_name_location::equal_sign;
          } else if (is_invalid_character_valid_provided(
                         cur_char, character_constants::g_k_valid_name_chars) ==
                     true) {
            switch (cur_char) {

            case character_constants::g_k_key_value_terminate: {
              std::string what_arg{"missing value part of key-value"};
              throw syntax_error::generate_formatted_error(m_file_contents,
                                                           m_cur_pos, what_arg);
            } break;

            default: {
              std::string what_arg{"invalid character in key name"};
              throw syntax_error::generate_formatted_error(m_file_contents,
                                                           m_cur_pos, what_arg);
            } break;
            }
          } else {
            key_name.push_back(cur_char);
          }
        }
      }
    } break;

    case key_name_location::trailing_whitespace: {
      if (m_cur_pos.is_eof() == true) {
        std::string what_arg{"missing value part of key-value"};
        throw syntax_error::generate_formatted_error(m_file_contents, m_cur_pos,
                                                     what_arg);
      } else {
        char cur_char{m_file_contents.get_char(m_cur_pos)};

        if (is_whitespace(cur_char) == true) {
          ;
        } else {
          if (cur_char == character_constants::g_k_key_value_assign) {
            last_state = key_name_location::equal_sign;
          } else {
            if (cur_char == character_constants::g_k_key_value_terminate) {
              std::string what_arg{"missing value part of key-value"};
              throw syntax_error::generate_formatted_error(m_file_contents,
                                                           m_cur_pos, what_arg);
            } else {
              std::string what_arg{"expected value assigment after key name"};
              throw syntax_error::generate_formatted_error(m_file_contents,
                                                           m_cur_pos, what_arg);
            }
          }
        }
      }
    } break;

    case key_name_location::equal_sign: {
      throw std::runtime_error{"impossible!"};
    } break;
    }
  }

  // m_cur_pos = equal sign

  return key_name;
}

libconfigfile::node_ptr<libconfigfile::value_node>
libconfigfile::parser::parse_key_value_value() {
  // m_cur_pos = equal sign

  std::string value_contents{};
  file_pos value_start_pos{m_cur_pos.get_end_of_file_pos()};

  {
    enum class value_location {
      equal_sign,
      leading_whitespace,
      value_proper,
      semicolon,
      done,
    };

    bool first_loop{true};
    bool in_string{false};
    char cur_char{'\0'};
    char last_char{'\0'};
    for (value_location last_state{value_location::equal_sign};
         last_state != value_location::done;
         ++m_cur_pos, first_loop = false, last_char = cur_char) {

      if (m_cur_pos.is_eof() == false) {
        cur_char = m_file_contents.get_char(m_cur_pos);
      }

      switch (last_state) {

      case value_location::equal_sign: {
        if (m_cur_pos.is_eof() == true) {
          std::string what_arg{"missing value part of key-value"};
          throw syntax_error::generate_formatted_error(m_file_contents,
                                                       m_cur_pos, what_arg);
        } else {
          if ((first_loop == true) &&
              (cur_char == character_constants::g_k_key_value_assign)) {
            ;
          } else {
            if (is_whitespace(cur_char)) {
              last_state = value_location::leading_whitespace;
            } else if (cur_char ==
                       character_constants::g_k_key_value_terminate) {
              last_state = value_location::semicolon;
              std::string what_arg{"empty value part of key-value"};
              throw syntax_error::generate_formatted_error(m_file_contents,
                                                           m_cur_pos, what_arg);
            } else {
              last_state = value_location::value_proper;
              value_start_pos = m_cur_pos;
              value_contents.push_back(cur_char);

              if (cur_char == character_constants::g_k_string_delimiter) {
                in_string = !in_string;
              }
            }
          }
        }
      } break;

      case value_location::leading_whitespace: {
        if (m_cur_pos.is_eof() == true) {
          std::string what_arg{"missing value part of key-value"};
          throw syntax_error::generate_formatted_error(m_file_contents,
                                                       m_cur_pos, what_arg);
        } else {
          char cur_char{m_file_contents.get_char(m_cur_pos)};

          if (is_whitespace(cur_char)) {
            ;
          } else if (cur_char == character_constants::g_k_key_value_terminate) {
            last_state = value_location::semicolon;
            std::string what_arg{"empty value part of key-value"};
            throw syntax_error::generate_formatted_error(m_file_contents,
                                                         m_cur_pos, what_arg);
          } else {
            last_state = value_location::value_proper;
            value_start_pos = m_cur_pos;
            value_contents.push_back(cur_char);

            if (cur_char == character_constants::g_k_string_delimiter) {
              in_string = !in_string;
            }
          }
        }
      } break;

      case value_location::value_proper: {
        if (m_cur_pos.is_eof() == true) {
          std::string what_arg{"unterminated key-value"};
          throw syntax_error::generate_formatted_error(m_file_contents,
                                                       m_cur_pos, what_arg);
        } else {
          char cur_char{m_file_contents.get_char(m_cur_pos)};

          if ((in_string == false) &&
              (cur_char == character_constants::g_k_key_value_terminate)) {
            last_state = value_location::semicolon;
          } else {
            value_contents.push_back(cur_char);

            if (cur_char == character_constants::g_k_string_delimiter) {
              if (in_string == false) {
                in_string = !in_string;
              } else if ((in_string == true) &&
                         (last_char !=
                          character_constants::g_k_escape_leader)) {
                in_string = !in_string;
              }
            }
          }
        }
      } break;

      case value_location::semicolon: {
        last_state = value_location::done;
      } break;

      case value_location::done: {
        throw std::runtime_error{"impossible!"};
      } break;
      }
    }
  }

  value_contents = trim_whitespace(
      value_contents, character_constants::g_k_whitespace_chars, false, true);

  node_ptr<value_node> ret_val{node_ptr_cast<value_node>(
      call_appropriate_value_parse_func(value_contents, value_start_pos))};

  return ret_val;
}

libconfigfile::node_ptr<libconfigfile::array_value_node>
libconfigfile::parser::parse_array_value(const std::string &raw_value,
                                         const file_pos &start_pos) {
  // start_pos = first char of raw value

  enum class char_type {
    leading_whitespace,
    opening_delimiter,
    element_leading_whitespace,
    element_proper,
    element_trailing_whitespace,
    element_separator,
    closing_delimiter,
    trailing_whitespace,
    done,
  };

  char_type last_char_type{char_type::leading_whitespace};

  bool in_string{false};
  int in_n_levels_of_sub_arrays{0};
  char cur_char{'\0'};
  char last_char{'\0'};

  std::pair<std::string, file_pos> cur_raw_element{{}, start_pos};
  std::vector<std::pair<std::string, file_pos>> raw_elements{};

  for (std::string::size_type raw_value_idx{0};
       (raw_value_idx < raw_value.size()) &&
       (last_char_type != char_type::done);
       ++raw_value_idx, last_char = cur_char) {

    cur_char = raw_value[raw_value_idx];

    switch (last_char_type) {
    case char_type::leading_whitespace: {
      if (is_whitespace(cur_char) == true) {
        ;
      } else if (cur_char == character_constants::g_k_array_opening_delimiter) {
        last_char_type = char_type::opening_delimiter;
      } else {
        std::string what_arg{"expected array opening delimiter"};
        throw syntax_error::generate_formatted_error(
            m_file_contents, (start_pos + raw_value_idx), what_arg);
      }
    } break;

    case char_type::opening_delimiter: {
      if (is_whitespace(cur_char) == true) {
        last_char_type = char_type::element_leading_whitespace;
      } else {
        if (cur_char == character_constants::g_k_array_closing_delimiter) {
          last_char_type = char_type::closing_delimiter;
        } else if (cur_char ==
                   character_constants::g_k_array_element_separator) {
          std::string what_arg{"expected array element before separator"};
          throw syntax_error::generate_formatted_error(
              m_file_contents, (start_pos + raw_value_idx), what_arg);
        } else {
          last_char_type = char_type::element_proper;
          cur_raw_element.second = (start_pos + raw_value_idx);
          cur_raw_element.first.push_back(cur_char);

          if (cur_char == character_constants::g_k_string_delimiter) {
            in_string = !in_string;
          } else if (cur_char ==
                     character_constants::g_k_array_opening_delimiter) {
            ++in_n_levels_of_sub_arrays;
          }
        }
      }
    } break;

    case char_type::element_leading_whitespace: {
      if (is_whitespace(cur_char) == true) {
        ;
      } else {
        if (cur_char == character_constants::g_k_array_closing_delimiter) {
          last_char_type = char_type::closing_delimiter;
        } else if (cur_char ==
                   character_constants::g_k_array_element_separator) {
          std::string what_arg{"expected array element before separator"};
          throw syntax_error::generate_formatted_error(
              m_file_contents, (start_pos + raw_value_idx), what_arg);
        } else {
          last_char_type = char_type::element_proper;
          cur_raw_element.second = (start_pos + raw_value_idx);
          cur_raw_element.first.push_back(cur_char);

          if (cur_char == character_constants::g_k_string_delimiter) {
            in_string = !in_string;
          } else if (cur_char ==
                     character_constants::g_k_array_opening_delimiter) {
            ++in_n_levels_of_sub_arrays;
          }
        }
      }
    } break;

    case char_type::element_proper: {
      if ((cur_char == character_constants::g_k_array_element_separator) &&
          (in_string == false) && (in_n_levels_of_sub_arrays == 0)) {
        last_char_type = char_type::element_separator;
        raw_elements.push_back(std::move(cur_raw_element));
        cur_raw_element.first.clear();
      } else if ((cur_char ==
                  character_constants::g_k_array_closing_delimiter) &&
                 (in_string == false) && (in_n_levels_of_sub_arrays == 0)) {
        last_char_type = char_type::closing_delimiter;
        raw_elements.push_back(std::move(cur_raw_element));
        cur_raw_element.first.clear();
      } else {
        cur_raw_element.first.push_back(cur_char);

        if (cur_char == character_constants::g_k_string_delimiter) {
          if (in_string == false) {
            in_string = !in_string;
          } else if ((in_string == true) &&
                     (last_char != character_constants::g_k_escape_leader)) {
            in_string = !in_string;
          }
        } else if ((cur_char ==
                    character_constants::g_k_array_opening_delimiter) &&
                   (in_string == false)) {
          ++in_n_levels_of_sub_arrays;
        } else if ((cur_char ==
                    character_constants::g_k_array_closing_delimiter) &&
                   (in_string == false)) {
          --in_n_levels_of_sub_arrays;
        }
      }
    } break;

    case char_type::element_trailing_whitespace: {
      if (is_whitespace(cur_char) == true) {
        ;
      } else {
        if (cur_char == character_constants::g_k_array_closing_delimiter) {
          last_char_type = char_type::closing_delimiter;
        } else if (cur_char ==
                   character_constants::g_k_array_element_separator) {
          last_char_type = char_type::element_separator;
        } else {
          std::string what_arg{
              "expected array closing delimiter or element separator"};
          throw syntax_error::generate_formatted_error(
              m_file_contents, (start_pos + raw_value_idx), what_arg);
        }
      }
    } break;

    case char_type::element_separator: {
      if (is_whitespace(cur_char) == true) {
        last_char_type = char_type::element_leading_whitespace;
      } else {
        if (cur_char == character_constants::g_k_array_closing_delimiter) {
          last_char_type = char_type::closing_delimiter;
        } else if (cur_char ==
                   character_constants::g_k_array_element_separator) {
          std::string what_arg{"expected array element or closing delimiter"};
          throw syntax_error::generate_formatted_error(
              m_file_contents, (start_pos + raw_value_idx), what_arg);
        } else {
          last_char_type = char_type::element_proper;
          cur_raw_element.second = (start_pos + raw_value_idx);
          cur_raw_element.first.push_back(cur_char);

          if (cur_char == character_constants::g_k_string_delimiter) {
            in_string = !in_string;
          } else if (cur_char ==
                     character_constants::g_k_array_opening_delimiter) {
            ++in_n_levels_of_sub_arrays;
          }
        }
      }
    } break;

    case char_type::closing_delimiter: {
      if (is_whitespace(cur_char) == true) {
        last_char_type = char_type::trailing_whitespace;
      } else {
        std::string what_arg{"extraneous character(s) after array"};
        throw syntax_error::generate_formatted_error(
            m_file_contents, (start_pos + raw_value_idx), what_arg);
      }
    } break;

    case char_type::trailing_whitespace: {
      if (is_whitespace(cur_char) == true) {
        ;
      } else {
        std::string what_arg{"extraneous character(s) after array"};
        throw syntax_error::generate_formatted_error(
            m_file_contents, (start_pos + raw_value_idx), what_arg);
      }
    } break;

    case char_type::done: {
      throw std::runtime_error{"impossible!"};
    } break;
    }
  }

  if (!((last_char_type == char_type::done) ||
        (last_char_type == char_type::closing_delimiter))) {
    std::string what_arg{"unterminated array"};
    throw syntax_error::generate_formatted_error(
        m_file_contents, (start_pos + (raw_value.size() - 1)), what_arg);
  }

  node_ptr<array_value_node> ret_val{make_node_ptr<array_value_node>()};
  ret_val->reserve(raw_elements.size());

  for (size_t i{0}; i < raw_elements.size(); ++i) {
    raw_elements[i].first =
        trim_whitespace(raw_elements[i].first,
                        character_constants::g_k_whitespace_chars, false, true);
  }

  for (size_t i{0}; i < raw_elements.size(); ++i) {
    ret_val->push_back(call_appropriate_value_parse_func(
        raw_elements[i].first, raw_elements[i].second));
  }

  return ret_val;
}

libconfigfile::node_ptr<libconfigfile::integer_end_value_node>
libconfigfile::parser::parse_integer_value(const std::string &raw_value,
                                           const file_pos &start_pos) {
  // start_pos = first char of raw value

  if (raw_value.empty()) {
    std::string what_arg{"empty value"};
    throw syntax_error::generate_formatted_error(m_file_contents, start_pos,
                                                 what_arg);
  } else {
    std::string actual_digits{};
    actual_digits.reserve(raw_value.size());
    bool is_negative{false};
    const numeral_system *num_sys{nullptr};

    bool last_char_was_digit{false};
    bool any_digits_so_far{false};

    bool last_char_was_leading_zero{false};
    size_t num_of_leading_zeroes{0};

    /* character_constants::g_k_hex_num_sys can't be constexpr because the
     * digits string is too long, this means that we can't use the prefix
     * members as case labels in the switch statement below, resulting in this
     * nasty workaround */
    static constexpr char constexpr_workaround_hex_num_sys_prefix{'x'};
    static constexpr char constexpr_workaround_hex_num_sys_prefix_alt{'X'};
    assert(constexpr_workaround_hex_num_sys_prefix ==
           character_constants::g_k_hex_num_sys.prefix);
    assert((constexpr_workaround_hex_num_sys_prefix_alt ==
            character_constants::g_k_hex_num_sys.prefix_alt));

    assert(character_constants::g_k_num_sys_prefix_leader == '0'); /* parsing
           logic breaks down if the numeral system prefix leader is not zero*/

    for (std::string::size_type raw_value_idx{0};
         raw_value_idx < raw_value.size(); ++raw_value_idx) {
      char cur_char{raw_value[raw_value_idx]};

      const auto default_char_behaviour{
          [&start_pos, &num_sys, &cur_char, &last_char_was_digit,
           &any_digits_so_far, &last_char_was_leading_zero, &actual_digits,
           &raw_value_idx, this]() {
            num_sys =
                ((num_sys == nullptr) ? (&character_constants::g_k_dec_num_sys)
                                      : (num_sys));

            if (is_digit(cur_char, *num_sys)) {
              last_char_was_digit = true;
              any_digits_so_far = true;
              last_char_was_leading_zero = false;
              actual_digits.push_back(cur_char);
            } else {
              last_char_was_digit = false;
              last_char_was_leading_zero = false;
              std::string what_arg{"invalid character in integer"};
              throw syntax_error::generate_formatted_error(
                  m_file_contents, (start_pos + raw_value_idx), what_arg);
            }
          }};

      switch (cur_char) {

      case character_constants::g_k_num_digit_separator: {
        if ((last_char_was_digit == false) ||
            (raw_value_idx == (raw_value.size() - 1))) {
          std::string what_arg{"integer digit separator must be durrounded by "
                               "at least one digit on each side"};
          throw syntax_error::generate_formatted_error(
              m_file_contents, (start_pos + raw_value_idx), what_arg);
        } else {
          last_char_was_digit = false;
          last_char_was_leading_zero = false;
        }
      } break;

      case character_constants::g_k_num_positive_sign: {
        if (raw_value_idx == 0) {
          is_negative = false;
          last_char_was_digit = false;
          last_char_was_leading_zero = false;
        } else {
          std::string what_arg{"positive sign must appear at start of integer"};
          throw syntax_error::generate_formatted_error(
              m_file_contents, (start_pos + raw_value_idx), what_arg);
        }
      } break;

      case character_constants::g_k_num_negative_sign: {
        if (raw_value_idx == 0) {
          is_negative = true;
          last_char_was_digit = false;
          last_char_was_leading_zero = false;
        } else {
          std::string what_arg{"negative sign must appear at start of integer"};
          throw syntax_error::generate_formatted_error(
              m_file_contents, (start_pos + raw_value_idx), what_arg);
        }
      } break;

      case character_constants::g_k_num_sys_prefix_leader: {
        if (any_digits_so_far == false) {
          last_char_was_digit = false;
          last_char_was_leading_zero = true;
          ++num_of_leading_zeroes;
        } else {
          last_char_was_digit = true;
          last_char_was_leading_zero = false;
          any_digits_so_far = true;
          actual_digits.push_back(cur_char);
        }
      } break;

      case character_constants::g_k_bin_num_sys.prefix:
      case character_constants::g_k_bin_num_sys.prefix_alt: {
        if (num_sys == nullptr) {
          if (any_digits_so_far == false) {
            if (last_char_was_leading_zero == true) {
              if (num_of_leading_zeroes == 1) {
                last_char_was_digit = false;
                last_char_was_leading_zero = false;

                num_sys = &character_constants::g_k_bin_num_sys;
              } else {
                std::string what_arg{"numeral system prefix must appear "
                                     "before integer digits"};
                throw syntax_error::generate_formatted_error(
                    m_file_contents, (start_pos + raw_value_idx), what_arg);
              }
            } else {
              std::string what_arg{"invalid character in integer"};
              throw syntax_error::generate_formatted_error(
                  m_file_contents, (start_pos + raw_value_idx), what_arg);
            }
          } else {
            std::string what_arg{
                "numeral system prefix must appear before integer digits"};
            throw syntax_error::generate_formatted_error(
                m_file_contents, (start_pos + raw_value_idx), what_arg);
          }
        } else {
          default_char_behaviour();
        }
      } break;

      case character_constants::g_k_oct_num_sys.prefix:
      case character_constants::g_k_oct_num_sys.prefix_alt: {
        if (num_sys == nullptr) {
          if (any_digits_so_far == false) {
            if (last_char_was_leading_zero == true) {
              if (num_of_leading_zeroes == 1) {
                last_char_was_digit = false;
                last_char_was_leading_zero = false;

                num_sys = &character_constants::g_k_oct_num_sys;
              } else {
                std::string what_arg{"numeral system prefix must appear "
                                     "before integer digits"};
                throw syntax_error::generate_formatted_error(
                    m_file_contents, (start_pos + raw_value_idx), what_arg);
              }
            } else {
              std::string what_arg{"invalid character in integer"};
              throw syntax_error::generate_formatted_error(
                  m_file_contents, (start_pos + raw_value_idx), what_arg);
            }
          } else {
            std::string what_arg{
                "numeral system prefix must appear before integer digits"};
            throw syntax_error::generate_formatted_error(
                m_file_contents, (start_pos + raw_value_idx), what_arg);
          }
        } else {
          default_char_behaviour();
        }
      } break;

      case constexpr_workaround_hex_num_sys_prefix:
      case constexpr_workaround_hex_num_sys_prefix_alt: {
        if (num_sys == nullptr) {
          if (any_digits_so_far == false) {
            if (last_char_was_leading_zero == true) {
              if (num_of_leading_zeroes == 1) {
                last_char_was_digit = false;
                last_char_was_leading_zero = false;

                num_sys = &character_constants::g_k_hex_num_sys;
              } else {
                std::string what_arg{"numeral system prefix must appear "
                                     "before integer digits"};
                throw syntax_error::generate_formatted_error(
                    m_file_contents, (start_pos + raw_value_idx), what_arg);
              }
            } else {
              std::string what_arg{"invalid character in integer"};
              throw syntax_error::generate_formatted_error(
                  m_file_contents, (start_pos + raw_value_idx), what_arg);
            }
          } else {
            std::string what_arg{
                "numeral system prefix must appear before integer digits"};
            throw syntax_error::generate_formatted_error(
                m_file_contents, (start_pos + raw_value_idx), what_arg);
          }
        } else {
          default_char_behaviour();
        }
      } break;

      default: {
        default_char_behaviour();
      } break;
      }
    }

    if (actual_digits.empty()) {
      actual_digits = "0";
    }

    if (num_sys == nullptr) {
      num_sys = &character_constants::g_k_dec_num_sys;
    }

    node_ptr<integer_end_value_node> ret_val{nullptr};

    static_assert((sizeof(decltype(std::stoll(""))) >=
                   sizeof(integer_end_value_node_data_t)),
                  "no string-to-int conversion function (std::stoi(), "
                  "std::stol(), std::stoll()) with return type large "
                  "enough for integer_end_value_node_t");

    try {
      if constexpr ((sizeof(decltype(std::stoi("")))) >=
                    (sizeof(integer_end_value_node_data_t))) {
        ret_val = make_node_ptr<integer_end_value_node>(
            std::stoi(actual_digits, nullptr, num_sys->base));
      } else if constexpr ((sizeof(decltype(std::stol("")))) >=
                           (sizeof(integer_end_value_node_data_t))) {
        ret_val = make_node_ptr<integer_end_value_node>(
            std::stol(actual_digits, nullptr, num_sys->base));
      } else {
        ret_val = make_node_ptr<integer_end_value_node>(
            std::stoll(actual_digits, nullptr, num_sys->base));
      }
    } catch (const std::out_of_range &ex) {
      std::string what_arg{"integer value is too large"};
      throw syntax_error::generate_formatted_error(m_file_contents, start_pos,
                                                   what_arg);
    }

    if (is_negative == true) {
      ret_val->set(-(ret_val->get()));
    }

    return ret_val;
  }
}

libconfigfile::node_ptr<libconfigfile::float_end_value_node>
libconfigfile::parser::parse_float_value(const std::string &raw_value,
                                         const file_pos &start_pos) {
  // start_pos = first char of raw value

  if (raw_value.empty()) {
    std::string what_arg{"empty value"};
    throw syntax_error::generate_formatted_error(m_file_contents, start_pos,
                                                 what_arg);
  } else {
    std::string sanitized_string{};
    sanitized_string.reserve(raw_value.size());

    static const std::unordered_map<std::string, float_end_value_node_data_t>
        special_floats{{character_constants::g_k_float_infinity.second,
                        character_constants::g_k_float_infinity.first},
                       {(character_constants::g_k_num_positive_sign +
                         character_constants::g_k_float_infinity.second),
                        (character_constants::g_k_float_infinity.first)},
                       {(character_constants::g_k_num_negative_sign +
                         character_constants::g_k_float_infinity.second),
                        (-character_constants::g_k_float_infinity.first)},
                       {(character_constants::g_k_float_not_a_number.second),
                        (character_constants::g_k_float_not_a_number.first)},
                       {(character_constants::g_k_num_positive_sign +
                         character_constants::g_k_float_not_a_number.second),
                        (character_constants::g_k_float_not_a_number.first)},
                       {(character_constants::g_k_num_negative_sign +
                         character_constants::g_k_float_not_a_number.second),
                        (-character_constants::g_k_float_not_a_number.first)}};

    if (special_floats.contains(raw_value)) {
      return make_node_ptr<float_end_value_node>(special_floats.at(raw_value));
    } else {

      enum class char_type {
        start,
        digit,
        positive,
        negative,
        decimal,
        exponent,
        separator,
      };

      char_type last_char{char_type::start};

      enum class num_location {
        integer,
        fractional,
        exponent,
      };

      num_location cur_location{num_location::integer};

      const std::string::size_type last_raw_value_idx{raw_value.size() - 1};

      for (std::string::size_type raw_value_idx{0};
           raw_value_idx < raw_value.size(); ++raw_value_idx) {
        char cur_char{raw_value[raw_value_idx]};

        switch (cur_location) {

        case num_location::integer: {
          switch (cur_char) {

          case character_constants::g_k_num_positive_sign: {
            if (last_char == char_type::start) {
              last_char = char_type::positive;
              sanitized_string.push_back(cur_char);
            } else {
              std::string what_arg{"positive sign may only appear at start of "
                                   "integer part or exponent part of float"};
              throw syntax_error::generate_formatted_error(
                  m_file_contents, (start_pos + raw_value_idx), what_arg);
            }
          } break;

          case character_constants::g_k_num_negative_sign: {
            if (last_char == char_type::start) {
              last_char = char_type::positive;
              sanitized_string.push_back(cur_char);
            } else {
              std::string what_arg{"negative sign may only appear at start of "
                                   "integer part or exponent part of float"};
              throw syntax_error::generate_formatted_error(
                  m_file_contents, (start_pos + raw_value_idx), what_arg);
            }
          } break;

          case character_constants::g_k_num_digit_separator: {
            if (last_char == char_type::digit) {
              if (raw_value_idx != last_raw_value_idx) {
                char next_char{raw_value[raw_value_idx + 1]};
                if (is_digit(next_char, character_constants::g_k_dec_num_sys) ==
                    true) {
                  last_char = char_type::separator;
                } else {
                  std::string what_arg{
                      "float digit separator must be surrounded by "
                      "at least one digit on each side"};
                  throw syntax_error::generate_formatted_error(
                      m_file_contents, (start_pos + raw_value_idx), what_arg);
                }
              } else {
                std::string what_arg{
                    "float digit separator must be surrounded by "
                    "at least one digit on each side"};
                throw syntax_error::generate_formatted_error(
                    m_file_contents, (start_pos + raw_value_idx), what_arg);
              }
            } else {
              std::string what_arg{
                  "float digit separator must be surrounded by "
                  "at least one digit on each side"};
              throw syntax_error::generate_formatted_error(
                  m_file_contents, (start_pos + raw_value_idx), what_arg);
            }
          } break;

          case character_constants::g_k_float_decimal_point: {
            if (last_char == char_type::digit) {
              if (raw_value_idx != last_raw_value_idx) {
                char next_char{raw_value[raw_value_idx + 1]};
                if (is_digit(next_char, character_constants::g_k_dec_num_sys) ==
                    true) {
                  last_char = char_type::decimal;
                  cur_location = num_location::fractional;
                  sanitized_string.push_back(cur_char);
                } else {
                  std::string what_arg{
                      "float decimal point must be surrounded by at "
                      "least one digit on each side"};
                  throw syntax_error::generate_formatted_error(
                      m_file_contents, (start_pos + raw_value_idx), what_arg);
                }
              } else {
                std::string what_arg{
                    "float decimal point must be surrounded by at "
                    "least one digit on each side"};
                throw syntax_error::generate_formatted_error(
                    m_file_contents, (start_pos + raw_value_idx), what_arg);
              }
            } else {
              std::string what_arg{
                  "float decimal point must be surrounded by at "
                  "least one digit on each side"};
              throw syntax_error::generate_formatted_error(
                  m_file_contents, (start_pos + raw_value_idx), what_arg);
            }
          } break;

          case character_constants::g_k_float_exponent_sign_lower:
          case character_constants::g_k_float_exponent_sign_upper: {
            if (last_char == char_type::digit) {
              if (raw_value_idx != last_raw_value_idx) {
                char next_char{raw_value[raw_value_idx + 1]};
                if ((is_digit(next_char,
                              character_constants::g_k_dec_num_sys) == true) ||
                    (next_char == character_constants::g_k_num_positive_sign) ||
                    (character_constants::g_k_num_negative_sign)) {
                  last_char = char_type::exponent;
                  cur_location = num_location::exponent;
                  sanitized_string.push_back(cur_char);
                } else {
                  std::string what_arg{
                      "float exponent sign must be surrounded by at "
                      "least on digit on each side"};
                  throw syntax_error::generate_formatted_error(
                      m_file_contents, (start_pos + raw_value_idx), what_arg);
                }
              } else {
                std::string what_arg{
                    "float exponent sign must be surrounded by at "
                    "least on digit on each side"};
                throw syntax_error::generate_formatted_error(
                    m_file_contents, (start_pos + raw_value_idx), what_arg);
              }
            } else {
              std::string what_arg{
                  "float exponent sign must be surrounded by at "
                  "least on digit on each side"};
              throw syntax_error::generate_formatted_error(
                  m_file_contents, (start_pos + raw_value_idx), what_arg);
            }
          } break;

          default: {
            if (is_digit(cur_char, character_constants::g_k_dec_num_sys) ==
                true) {
              last_char = char_type::digit;
              sanitized_string.push_back(cur_char);
            } else {
              std::string what_arg{"invalid character in float"};
              throw syntax_error::generate_formatted_error(
                  m_file_contents, (start_pos + raw_value_idx), what_arg);
            }
          } break;
          }
        } break;

        case num_location::fractional: {
          switch (cur_char) {

          case character_constants::g_k_num_positive_sign: {
            std::string what_arg{"positive sign may only appear at start of "
                                 "integer part or exponent part of float"};
            throw syntax_error::generate_formatted_error(
                m_file_contents, (start_pos + raw_value_idx), what_arg);
          } break;

          case character_constants::g_k_num_negative_sign: {
            std::string what_arg{"negative sign may only appear at start of "
                                 "integer part or exponent part of float"};
            throw syntax_error::generate_formatted_error(
                m_file_contents, (start_pos + raw_value_idx), what_arg);
          } break;

          case character_constants::g_k_num_digit_separator: {
            if (last_char == char_type::digit) {
              if (raw_value_idx != last_raw_value_idx) {
                char next_char{raw_value[raw_value_idx + 1]};
                if (is_digit(next_char, character_constants::g_k_dec_num_sys) ==
                    true) {
                  last_char = char_type::separator;
                } else {
                  std::string what_arg{
                      "float digit separator must be surrounded by "
                      "at least one digit on each side"};
                  throw syntax_error::generate_formatted_error(
                      m_file_contents, (start_pos + raw_value_idx), what_arg);
                }
              } else {
                std::string what_arg{
                    "float digit separator must be surrounded by "
                    "at least one digit on each side"};
                throw syntax_error::generate_formatted_error(
                    m_file_contents, (start_pos + raw_value_idx), what_arg);
              }
            } else {
              std::string what_arg{
                  "float digit separator must be surrounded by "
                  "at least one digit on each side"};
              throw syntax_error::generate_formatted_error(
                  m_file_contents, (start_pos + raw_value_idx), what_arg);
            }
          } break;

          case character_constants::g_k_float_decimal_point: {
            std::string what_arg{"floats can only contain one decimal point"};
            throw syntax_error::generate_formatted_error(
                m_file_contents, (start_pos + raw_value_idx), what_arg);
          } break;

          case character_constants::g_k_float_exponent_sign_lower:
          case character_constants::g_k_float_exponent_sign_upper: {
            if (last_char == char_type::digit) {
              if (raw_value_idx != last_raw_value_idx) {
                char next_char{raw_value[raw_value_idx + 1]};
                if ((is_digit(next_char,
                              character_constants::g_k_dec_num_sys) == true) ||
                    (next_char == character_constants::g_k_num_positive_sign) ||
                    (character_constants::g_k_num_negative_sign)) {
                  last_char = char_type::exponent;
                  cur_location = num_location::exponent;
                  sanitized_string.push_back(cur_char);
                } else {
                  std::string what_arg{
                      "float exponent sign must be surrounded by at "
                      "least on digit on each side"};
                  throw syntax_error::generate_formatted_error(
                      m_file_contents, (start_pos + raw_value_idx), what_arg);
                }
              } else {
                std::string what_arg{
                    "float exponent sign must be surrounded by at "
                    "least on digit on each side"};
                throw syntax_error::generate_formatted_error(
                    m_file_contents, (start_pos + raw_value_idx), what_arg);
              }
            } else {
              std::string what_arg{
                  "float exponent sign must be surrounded by at "
                  "least on digit on each side"};
              throw syntax_error::generate_formatted_error(
                  m_file_contents, (start_pos + raw_value_idx), what_arg);
            }
          } break;

          default: {
            if (is_digit(cur_char, character_constants::g_k_dec_num_sys) ==
                true) {
              last_char = char_type::digit;
              sanitized_string.push_back(cur_char);
            } else {
              std::string what_arg{"invalid character in float"};
              throw syntax_error::generate_formatted_error(
                  m_file_contents, (start_pos + raw_value_idx), what_arg);
            }
          } break;
          }
        } break;

        case num_location::exponent: {
          switch (cur_char) {

          case character_constants::g_k_num_positive_sign: {
            if (last_char == char_type::exponent) {
              last_char = char_type::positive;
              sanitized_string.push_back(cur_char);
            } else {
              std::string what_arg{"positive sign may only appear at start of "
                                   "integer part or exponent part of float"};
              throw syntax_error::generate_formatted_error(
                  m_file_contents, (start_pos + raw_value_idx), what_arg);
            }
          } break;

          case character_constants::g_k_num_negative_sign: {
            if (last_char == char_type::exponent) {
              last_char = char_type::positive;
              sanitized_string.push_back(cur_char);
            } else {
              std::string what_arg{"negative sign may only appear at start of "
                                   "integer part or exponent part of float"};
              throw syntax_error::generate_formatted_error(
                  m_file_contents, (start_pos + raw_value_idx), what_arg);
            }
          } break;

          case character_constants::g_k_num_digit_separator: {
            if (last_char == char_type::digit) {
              if (raw_value_idx != last_raw_value_idx) {
                char next_char{raw_value[raw_value_idx + 1]};
                if (is_digit(next_char, character_constants::g_k_dec_num_sys) ==
                    true) {
                  last_char = char_type::separator;
                } else {
                  std::string what_arg{
                      "float digit separator must be surrounded by "
                      "at least one digit on each side"};
                  throw syntax_error::generate_formatted_error(
                      m_file_contents, (start_pos + raw_value_idx), what_arg);
                }
              } else {
                std::string what_arg{
                    "float digit separator must be surrounded by "
                    "at least one digit on each side"};
                throw syntax_error::generate_formatted_error(
                    m_file_contents, (start_pos + raw_value_idx), what_arg);
              }
            } else {
              std::string what_arg{
                  "float digit separator must be surrounded by "
                  "at least one digit on each side"};
              throw syntax_error::generate_formatted_error(
                  m_file_contents, (start_pos + raw_value_idx), what_arg);
            }
          } break;

          case character_constants::g_k_float_decimal_point: {
            std::string what_arg{
                "float decimal point can not appear after exponent sign"};
            throw syntax_error::generate_formatted_error(
                m_file_contents, (start_pos + raw_value_idx), what_arg);
          } break;

          case character_constants::g_k_float_exponent_sign_lower:
          case character_constants::g_k_float_exponent_sign_upper: {
            std::string what_arg{"float exponent sign can only appear once"};
            throw syntax_error::generate_formatted_error(
                m_file_contents, (start_pos + raw_value_idx), what_arg);
          } break;

          default: {
            if (is_digit(cur_char, character_constants::g_k_dec_num_sys) ==
                true) {
              last_char = char_type::digit;
              sanitized_string.push_back(cur_char);
            } else {
              std::string what_arg{"invalid character in float"};
              throw syntax_error::generate_formatted_error(
                  m_file_contents, (start_pos + raw_value_idx), what_arg);
            }
          } break;
          }
        } break;
        }
      }

      if (sanitized_string.empty()) {
        sanitized_string = "0";
      }

      node_ptr<float_end_value_node> ret_val{nullptr};

      static_assert(
          (sizeof(decltype(std::stod(""))) >=
           sizeof(float_end_value_node_data_t)),
          "no string-to-float conversion function with return type large "
          "enough "
          "for float_end_value_node_t");

      try {
        if constexpr ((sizeof(decltype(std::stof("")))) >=
                      (sizeof(float_end_value_node_data_t))) {
          ret_val = make_node_ptr<float_end_value_node>(
              std::stof(sanitized_string, nullptr));
        } else if constexpr ((sizeof(decltype(std::stod("")))) >=
                             (sizeof(float_end_value_node_data_t))) {
          ret_val = make_node_ptr<float_end_value_node>(
              std::stod(sanitized_string, nullptr));
        } else {
          ret_val = make_node_ptr<float_end_value_node>(
              std::stold(sanitized_string, nullptr));
        }

      } catch (const std::out_of_range &ex) {
        std::string what_arg{"float value is too large"};
        throw syntax_error::generate_formatted_error(m_file_contents, start_pos,
                                                     what_arg);
      }

      return ret_val;
    }
  }
}

libconfigfile::node_ptr<libconfigfile::string_end_value_node>
libconfigfile::parser::parse_string_value(const std::string &raw_value,
                                          const file_pos &start_pos) {
  // start_pos = first char of raw value

  if (raw_value.empty()) {
    std::string what_arg{"empty value"};
    throw syntax_error::generate_formatted_error(m_file_contents, start_pos,
                                                 what_arg);
  } else {
    bool in_string{false};

    enum char_type {
      start,
      in_string_regular_char,
      in_string_escape_leader,
      in_string_escape_code,
      opening_delimiter,
      closing_delimiter,
      out_string_whitespace,
    };

    char_type last_char{char_type::start};

    std::string string_contents{};
    string_contents.reserve(raw_value.size());

    for (std::string::size_type raw_value_idx{};
         raw_value_idx < raw_value.size(); ++raw_value_idx) {
      char cur_char{raw_value[raw_value_idx]};
      if (in_string == true) {
        if (cur_char == character_constants::g_k_string_delimiter) {
          last_char = closing_delimiter;
          in_string = false;
        } else if (cur_char == character_constants::g_k_escape_leader) {
          std::string::size_type escape_char_pos{raw_value_idx + 1};
          if (escape_char_pos < raw_value.size()) {
            char escape_char{raw_value[escape_char_pos]};

            if (escape_char == character_constants::g_k_hex_escape_char) {
              std::string::size_type hex_digit_pos_1{escape_char_pos + 1};
              std::string::size_type hex_digit_pos_2{escape_char_pos + 2};

              if ((hex_digit_pos_1 < raw_value.size()) &&
                  (hex_digit_pos_2 < raw_value.size())) {
                char hex_digit_1{raw_value[hex_digit_pos_1]};
                char hex_digit_2{raw_value[hex_digit_pos_2]};

                if ((is_digit(hex_digit_1,
                              character_constants::g_k_hex_num_sys)) &&
                    (is_digit(hex_digit_2,
                              character_constants::g_k_hex_num_sys))) {
                  std::string hex_string{std::string{} + hex_digit_1 +
                                         hex_digit_2};
                  string_contents.push_back(static_cast<char>(
                      std::stoi(hex_string, nullptr,
                                character_constants::g_k_hex_num_sys.base)));
                  last_char = char_type::in_string_escape_code;
                  raw_value_idx = hex_digit_2;
                } else {
                  std::string what_arg{
                      "invalid digit in hexadecimal escape sequence"};
                  throw syntax_error::generate_formatted_error(
                      m_file_contents, (start_pos + raw_value_idx), what_arg);
                }
              } else {
                std::string what_arg{"incomplete escape sequence in string"};
                throw syntax_error::generate_formatted_error(
                    m_file_contents, (start_pos + raw_value_idx), what_arg);
              }
            } else {
              if (character_constants::g_k_basic_escape_chars.contains(
                      escape_char)) {
                string_contents.push_back(
                    character_constants::g_k_basic_escape_chars.at(
                        escape_char));
                last_char = char_type::in_string_escape_code;
                raw_value_idx = escape_char_pos;
              }
            }
          } else {
            std::string what_arg{"incomplete escape sequence in string"};
            throw syntax_error::generate_formatted_error(
                m_file_contents, (start_pos + raw_value_idx), what_arg);
          }
        } else {
          string_contents.push_back(cur_char);
        }
      } else {
        if (is_whitespace(cur_char,
                          character_constants::g_k_whitespace_chars) == true) {
          last_char = char_type::out_string_whitespace;
        } else if (cur_char == character_constants::g_k_string_delimiter) {
          last_char = char_type::opening_delimiter;
          in_string = true;
        } else {
          std::string what_arg{"invalid character outside of string"};
          throw syntax_error::generate_formatted_error(
              m_file_contents, (start_pos + raw_value_idx), what_arg);
        }
      }
    }

    if (in_string == true) {
      std::string what_arg{"unterminated string"};
      throw syntax_error::generate_formatted_error(
          m_file_contents, (start_pos + (raw_value.size() - 1)), what_arg);
    } else {
      return make_node_ptr<string_end_value_node>(string_contents);
    }
  }
}

libconfigfile::node_ptr<libconfigfile::value_node>
libconfigfile::parser::call_appropriate_value_parse_func(
    const std::string &raw_value, const file_pos &start_pos) {

  std::variant<value_node_type, end_value_node_type> value_type_variant{
      identify_key_value_value_type(raw_value)};

  switch (value_type_variant.index()) {

  case 0: {
    value_node_type value_type_extracted{std::get<0>(value_type_variant)};

    switch (value_type_extracted) {

    case value_node_type::ARRAY: {
      return node_ptr_cast<value_node>(parse_array_value(raw_value, start_pos));
    } break;

    default: {
      throw std::runtime_error{
          "invalid value type returned by identify_key_value_value_type()"};
    } break;
    }
  } break;

  case 1: {
    end_value_node_type value_type_extracted{std::get<1>(value_type_variant)};

    switch (value_type_extracted) {

    case end_value_node_type::STRING: {
      return node_ptr_cast<value_node>(
          parse_string_value(raw_value, start_pos));
    } break;

    case end_value_node_type::INTEGER: {
      return node_ptr_cast<value_node>(
          parse_integer_value(raw_value, start_pos));
    } break;

    case end_value_node_type::FLOAT: {
      return node_ptr_cast<value_node>(parse_float_value(raw_value, start_pos));
    } break;

    default: {
      throw std::runtime_error{
          "invalid value type returned by identify_key_value_value_type()"};
    } break;
    }
  } break;

  default: {
    throw std::runtime_error{"impossible"};
  } break;
  }
}

void libconfigfile::parser::parse_directive() {
  // m_cur_pos = directive leader
  // caller must check that directive is the only text on its line

  file_pos start_pos{m_cur_pos};

  std::string name{};
  name.reserve(character_constants::g_k_max_directive_name_length);

  enum class name_location {
    directive_leader,
    leading_whitespace,
    name_proper,
    done,
  };

  for (name_location last_state{name_location::directive_leader};
       last_state != name_location::done; ++m_cur_pos) {
    switch (last_state) {
    case name_location::directive_leader: {
      if (m_cur_pos.is_eof() == true) {
        std::string what_arg{"expected directive name"};
        throw syntax_error::generate_formatted_error(m_file_contents, m_cur_pos,
                                                     what_arg);
      } else {
        char cur_char{m_file_contents.get_char(m_cur_pos)};

        if (cur_char == character_constants::g_k_directive_leader) {
          ;
        } else if (is_whitespace(cur_char,
                                 character_constants::g_k_whitespace_chars) ==
                   true) {
          last_state = name_location::leading_whitespace;
          ;
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
        throw syntax_error::generate_formatted_error(m_file_contents, m_cur_pos,
                                                     what_arg);
      } else {
        char cur_char{m_file_contents.get_char(m_cur_pos)};

        if (is_whitespace(cur_char,
                          character_constants::g_k_whitespace_chars) == true) {
          ;
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
      } else {
        char cur_char{m_file_contents.get_char(m_cur_pos)};

        if (is_whitespace(cur_char,
                          character_constants::g_k_whitespace_chars) == true) {
          last_state = name_location::done;
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
      throw std::runtime_error{"impossible!"};
    } break;
    }
  }

  void (parser::*directive_function)(){nullptr};

  if (name == character_constants::g_k_version_directive_name) {
    directive_function = &parser::parse_version_directive;
  } else if (name == character_constants::g_k_include_directive_name) {
    directive_function = &parser::parse_include_directive;
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

      if (is_whitespace(cur_char, character_constants::g_k_whitespace_chars) ==
          true) {
        ;
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

void libconfigfile::parser::parse_include_directive() {
  // m_cur_pos = start of directive arguments
  // or eof if arguments don't exist

  file_pos start_pos{m_cur_pos};

  std::string file_path{};

  enum class args_location {
    leading_whitespace,
    opening_delimiter,
    file_path,
    closing_delimiter,
    trailing_whitespace,
    done,
  };

  file_pos start_of_file_path_pos{m_cur_pos.get_end_of_file_pos()};

  bool last_char_was_escape_leader{false};

  for (args_location last_state{args_location::leading_whitespace};
       last_state != args_location::done; ++m_cur_pos) {
    switch (last_state) {
    case args_location::leading_whitespace: {
      if (m_cur_pos.is_eof() == true) {
        std::string what_arg{"include directive requires file path argument"};
        throw syntax_error::generate_formatted_error(m_file_contents, m_cur_pos,
                                                     what_arg);
      } else {
        char cur_char{m_file_contents.get_char(m_cur_pos)};

        if (is_whitespace(cur_char,
                          character_constants::g_k_whitespace_chars) == true) {
          ;
        } else if (cur_char == character_constants::g_k_string_delimiter) {
          if (m_cur_pos.get_line() != start_pos.get_line()) {
            std::string what_arg{"entire directive must appear on one line"};
            throw syntax_error::generate_formatted_error(m_file_contents,
                                                         m_cur_pos, what_arg);
          } else {
            last_state = args_location::opening_delimiter;
          }
        } else {
          std::string what_arg{"include directive requires file path argument"};
          throw syntax_error::generate_formatted_error(m_file_contents,
                                                       m_cur_pos, what_arg);
        }
      }
    } break;

    case args_location::opening_delimiter: {
      if (m_cur_pos.is_eof() == true) {
        std::string what_arg{
            "unterminated string in include directive argument"};
        throw syntax_error::generate_formatted_error(m_file_contents, m_cur_pos,
                                                     what_arg);
      } else {
        if (m_cur_pos.get_line() != start_pos.get_line()) {
          std::string what_arg{"entire directive must appear on one line"};
          throw syntax_error::generate_formatted_error(m_file_contents,
                                                       m_cur_pos, what_arg);
        } else {
          char cur_char{m_file_contents.get_char(m_cur_pos)};

          if (cur_char == character_constants::g_k_string_delimiter) {
            last_state = args_location::closing_delimiter;
            start_of_file_path_pos = m_cur_pos;
          } else if (cur_char == character_constants::g_k_escape_leader) {
            last_char_was_escape_leader = true;
            file_path.push_back(cur_char);
            last_state = args_location::file_path;
            start_of_file_path_pos = m_cur_pos;
          } else {
            file_path.push_back(cur_char);
            last_state = args_location::file_path;
            start_of_file_path_pos = m_cur_pos;
          }
        }
      }
    } break;

    case args_location::file_path: {
      if (m_cur_pos.is_eof() == true) {
        std::string what_arg{
            "unterminated string in include directive argument"};
        throw syntax_error::generate_formatted_error(m_file_contents, m_cur_pos,
                                                     what_arg);
      } else {
        if (m_cur_pos.get_line() != start_pos.get_line()) {
          std::string what_arg{"entire directive must appear on one line"};
          throw syntax_error::generate_formatted_error(m_file_contents,
                                                       m_cur_pos, what_arg);
        } else {
          char cur_char{m_file_contents.get_char(m_cur_pos)};

          if (cur_char == character_constants::g_k_string_delimiter) {
            if (last_char_was_escape_leader == true) {
              last_char_was_escape_leader = false;
              file_path.push_back(cur_char);
            } else {
              last_state = args_location::closing_delimiter;
            }
          } else if (cur_char == character_constants::g_k_escape_leader) {
            last_char_was_escape_leader = true;
            file_path.push_back(cur_char);
          } else {
            file_path.push_back(cur_char);
          }
        }
      }
    } break;

    case args_location::closing_delimiter: {
      if (m_cur_pos.is_eof() == true) {
        last_state = args_location::done;
      } else {
        if (m_cur_pos.get_line() != start_pos.get_line()) {
          last_state = args_location::done;
        } else {
          char cur_char{m_file_contents.get_char(m_cur_pos)};

          if (is_whitespace(cur_char,
                            character_constants::g_k_whitespace_chars) ==
              true) {
            last_state = args_location::trailing_whitespace;
          } else {
            std::string what_arg{"excess arguments given to include directive"};
            throw syntax_error::generate_formatted_error(m_file_contents,
                                                         m_cur_pos, what_arg);
          }
        }
      }
    } break;

    case args_location::trailing_whitespace: {
      if (m_cur_pos.is_eof() == true) {
        last_state = args_location::done;
      } else {
        if (m_cur_pos.get_line() != start_pos.get_line()) {
          last_state = args_location::done;
        } else {
          char cur_char{m_file_contents.get_char(m_cur_pos)};

          if (is_whitespace(cur_char,
                            character_constants::g_k_whitespace_chars) ==
              true) {
            ;
          } else {
            std::string what_arg{"excess arguments given to include directive"};
            throw syntax_error::generate_formatted_error(m_file_contents,
                                                         m_cur_pos, what_arg);
          }
        }
      }
    } break;

    case args_location::done: {
      throw std::runtime_error{"impossible!"};
    } break;
    }
  }

  if (file_path.empty() == true) {
    std::string what_arg{"empty file path argument given to include directive"};
    throw syntax_error::generate_formatted_error(
        m_file_contents, start_of_file_path_pos, what_arg);
  } else {
    std::variant<std::string, std::string::size_type> file_path_escaped{
        replace_escape_sequences(file_path)};

    switch (file_path_escaped.index()) {
    case 0: {
      file_path = std::get<std::string>(std::move(file_path_escaped));

      file included_file{file_path};

      // TODO vvv change this to work with streams

      std::remove_reference_t<decltype(m_file_contents.get_underlying())>::
          const_iterator include_pos_iter{
              m_file_contents.get_underlying().begin() + start_pos.get_line()};

      m_file_contents.get_underlying().erase(include_pos_iter);
      m_file_contents.get_underlying().insert(
          include_pos_iter,
          std::make_move_iterator(included_file.get_underlying().begin()),
          std::make_move_iterator(included_file.get_underlying().end()));

      m_cur_pos = m_file_contents.create_file_pos(m_cur_pos);
      m_cur_pos.set_char(0);
    } break;

    case 1: {
      file_pos invalid_escape_sequence_pos{start_of_file_path_pos};
      invalid_escape_sequence_pos.set_char(
          (invalid_escape_sequence_pos.get_char()) +
          (std::get<std::string::size_type>(file_path_escaped)));

      std::string what_arg{
          "invalid escape sequence in include directive argument"};
      throw syntax_error::generate_formatted_error(
          m_file_contents, invalid_escape_sequence_pos, what_arg);
    } break;
    }
  }
}

void libconfigfile::parser::handle_comments() {
  if (m_cur_pos.is_eof() == true) {
    return;
  } else {
    char cur_char{m_file_contents.get_char(m_cur_pos)};

    if (cur_char == character_constants::g_k_comment_script) {
      m_cur_pos.goto_next_line();
      return;
    } else if ((cur_char == character_constants::g_k_comment_cpp.front()) ||
               (cur_char == character_constants::g_k_comment_c_start.front())) {
      file_pos next_pos{m_cur_pos + 1};

      if (next_pos.is_eof() == true) {
        return;
      } else {
        char next_char{m_file_contents.get_char(next_pos)};

        if (next_char == character_constants::g_k_comment_cpp.back()) {
          m_cur_pos.goto_next_line();
          return;
        } else if (next_char ==
                   character_constants::g_k_comment_c_start.back()) {
          m_cur_pos.goto_find_end(character_constants::g_k_comment_c_end);
          return;
        } else {
          return;
        }
      }
    } else {
      return;
    }
  }
}

std::variant<libconfigfile::value_node_type, libconfigfile::end_value_node_type>
libconfigfile::parser::identify_key_value_value_type(
    const std::string &value_contents) {
  std::string::size_type first_non_whitespace_char_pos{
      value_contents.find_first_not_of(
          character_constants::g_k_whitespace_chars)};
  char first_non_whitespace_char{value_contents[first_non_whitespace_char_pos]};

  switch (first_non_whitespace_char) {

  case character_constants::g_k_array_opening_delimiter: {
    return value_node_type::ARRAY;
  } break;

  case character_constants::g_k_string_delimiter: {
    return end_value_node_type::STRING;
  } break;

  default: {
    return identify_key_value_numeric_value_type(value_contents);
  } break;
  }
}

libconfigfile::end_value_node_type
libconfigfile::parser::identify_key_value_numeric_value_type(
    const std::string &value_contents) {
  if ((case_insensitive_string_find(
           value_contents, character_constants::g_k_float_infinity.second) !=
       (std::string::npos)) ||
      (case_insensitive_string_find(
           value_contents,
           character_constants::g_k_float_not_a_number.second) !=
       (std::string::npos))) {
    return end_value_node_type::FLOAT;
  } else {
    if ((value_contents.find(character_constants::g_k_float_decimal_point)) !=
        (std::string::npos)) {
      return end_value_node_type::FLOAT;
    } else {
      if (((value_contents.find(
               character_constants::g_k_float_exponent_sign_lower)) !=
           (std::string::npos)) ||
          ((value_contents.find(
               character_constants::g_k_float_exponent_sign_upper)) !=
           (std::string::npos))) {
        if (((value_contents.find(
                 character_constants::g_k_hex_num_sys.prefix)) !=
             (std::string::npos)) ||
            ((value_contents.find(
                 character_constants::g_k_hex_num_sys.prefix_alt)) !=
             (std::string::npos))) {
          return end_value_node_type::INTEGER;
        } else {
          return end_value_node_type::FLOAT;
        }
      } else {
        return end_value_node_type::INTEGER;
      }
    }
  }
}

std::variant<std::string /*result*/,
             std::string::size_type /*invalid_escape_sequence_pos*/>
libconfigfile::parser::replace_escape_sequences(const std::string &str) {
  std::string result{};
  result.reserve(str.size());

  for (std::string::size_type cur_char{0}; cur_char < str.size(); ++cur_char) {
    if (str[cur_char] == character_constants::g_k_escape_leader) {
      std::string::size_type escape_char_pos{cur_char + 1};
      if (escape_char_pos < str.size()) {
        char escape_char{str[escape_char_pos]};
        if (escape_char == character_constants::g_k_hex_escape_char) {
          std::string::size_type hex_digit_pos_1{escape_char_pos + 1};
          std::string::size_type hex_digit_pos_2{escape_char_pos + 2};
          if ((hex_digit_pos_1 < str.size()) &&
              (hex_digit_pos_2 < str.size())) {
            char hex_digit_1{str[hex_digit_pos_1]};
            char hex_digit_2{str[hex_digit_pos_2]};
            if ((is_digit(hex_digit_1, character_constants::g_k_hex_num_sys)) &&
                (is_digit(hex_digit_2, character_constants::g_k_hex_num_sys))) {
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
          if (character_constants::g_k_basic_escape_chars.contains(
                  escape_char)) {
            result.push_back(
                character_constants::g_k_basic_escape_chars.at(escape_char));
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
    const std::string &raw,
    const char delimiter /*= character_constants::g_k_string_delimiter*/,
    const char delimiter_escape /*= character_constants::g_k_escape_leader*/,
    const std::string
        &whitespace_chars /*= character_constants::g_k_whitespace_chars*/) {

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
    const std::string
        &whitespace_chars /*= character_constants::g_k_whitespace_chars*/) {
  return ((whitespace_chars.find(ch)) != (std::string::npos));
}

std::string libconfigfile::parser::trim_whitespace(
    const std::string &str,
    const std::string
        &whitespace_chars /*= character_constants::g_k_whitespace_chars*/,
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
    const char
        delimiter_escape /*= character_constants::g_k_delimiter_leader*/) {
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

std::pair<bool, std::string::size_type>
libconfigfile::parser::contains_invalid_character_valid_provided(
    const std::string &str, const std::string &valid_chars) {
  for (size_t i{0}; i < str.size(); ++i) {
    if (is_invalid_character_valid_provided(str[i], valid_chars) == true) {
      return {true, i};
    }
  }
  return {false, std::string::npos};
}

std::pair<bool, std::string::size_type>
libconfigfile::parser::contains_invalid_character_invalid_provided(
    const std::string &str, const std::string &invalid_chars) {
  for (size_t i{0}; i < str.size(); ++i) {
    if (is_invalid_character_invalid_provided(str[i], invalid_chars) == true) {
      return {true, i};
    }
  }
  return {false, std::string::npos};
}

bool libconfigfile::parser::is_digit(
    char ch,
    const numeral_system &num_sys /*= character_constants::g_k_dec_num_sys*/) {
  return (num_sys.digits.find(ch) != std::string::npos);
}

bool libconfigfile::parser::case_insensitive_string_compare(
    const std::string &str1, const std::string &str2) {
  if (str1.size() == str2.size()) {
    for (std::string::size_type i{0}; i < str1.size(); ++i) {
      if (std::tolower(str1[i]) != std::tolower(str1[i])) {
        return false;
      }
    }
  } else {
    return false;
  }
  return true;
}

std::string::size_type libconfigfile::parser::case_insensitive_string_find(
    const std::string &str, const std::string &to_find) {
  static const auto case_insensitive_char_compare{
      [](std::string::value_type ch1, std::string::value_type ch2) -> bool {
        return ((std::tolower(ch1)) == (std::tolower(ch2)));
      }};

  auto found_iter{std::search(str.begin(), str.end(), to_find.begin(),
                              to_find.end(), case_insensitive_char_compare)};

  if (found_iter == str.end()) {
    return std::string::npos;
  } else {
    return ((found_iter) - (str.begin()));
  }
}

bool libconfigfile::parser::string_contains_only(const std::string &str,
                                                 const std::string &chars) {
  return ((str.find_first_not_of(chars)) == (std::string::npos));
}

bool libconfigfile::parser::string_contains_any_of(const std::string &str,
                                                   const std::string &chars) {
  return ((str.find_first_of(chars)) != (std::string::npos));
}

std::string libconfigfile::parser::string_to_upper(const std::string &str) {
  std::string ret_val{};
  ret_val.resize(str.size());

  for (size_t i{0}; i < str.size(); ++i) {
    ret_val[i] = std::toupper(str[i]);
  }

  return ret_val;
}

std::string libconfigfile::parser::string_to_lower(const std::string &str) {
  std::string ret_val{};
  ret_val.resize(str.size());

  for (size_t i{0}; i < str.size(); ++i) {
    ret_val[i] = std::tolower(str[i]);
  }

  return ret_val;
}
