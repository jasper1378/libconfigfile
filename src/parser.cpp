#include "parser.hpp"

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
#include "semantic_error.hpp"
#include "string_end_value_node.hpp"
#include "syntax_error.hpp"
#include "value_node.hpp"
#include "version.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

libconfigfile::node_ptr<libconfigfile::section_node>
libconfigfile::parser::parse(const std::filesystem::path &file_path) {
  return impl::parse(file_path);
}

libconfigfile::node_ptr<libconfigfile::section_node>
libconfigfile::parser::impl::parse(const std::filesystem::path &file_path) {
  context ctx{file_path, std::ifstream{file_path},
              make_node_ptr<section_node>()};

  if ((ctx.file.is_open() == false) || (ctx.file.good() == false)) {
    throw std::runtime_error{"file \"" + ctx.file_path.string() +
                             "\" could not be opened for "
                             "reading"};
  } else {
    return parse_section(ctx, true).second;
  }
}

std::pair<std::string, libconfigfile::node_ptr<libconfigfile::section_node>>
libconfigfile::parser::impl::parse_section(context &ctx, bool is_root_section) {
  std::pair<std::string, node_ptr<section_node>> ret_val{
      "", make_node_ptr<section_node>()};

  if (is_root_section == true) {
    std::ifstream::int_type first_char{ctx.file.peek()};
    if (first_char == std::ifstream::traits_type::eof()) {
      return ret_val;
    }
  }

  if (is_root_section == false) {

    enum class name_location {
      opening_delimiter,
      leading_whitespace,
      name_proper,
      trailing_whitespace,
      closing_delimiter,
    };

    std::ifstream::pos_type start_of_name_proper_pos{};

    bool first_loop{true};

    for (name_location last_state{name_location::opening_delimiter};
         last_state != name_location::closing_delimiter; first_loop = false) {
      char cur_char{};
      bool eof{false};
      std::ifstream::pos_type cur_pos{};
      std::ifstream::pos_type last_newline_pos{};
      while (true) {
        cur_pos = ctx.file.tellg();
        if (ctx.file.eof() == true) {
          eof = true;
          break;
        } else {
          ctx.file.get(cur_char);
          if (cur_char == character_constants::g_k_newline) {
            last_newline_pos = cur_pos;
            continue;
          } else {
            break;
          }
        }
      }

      switch (last_state) {
      case name_location::opening_delimiter: {
        if (eof == true) {
          std::string what_arg{"expected section name"};
          throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                       cur_pos);
        } else {
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
                    what_arg, ctx.file_path, cur_pos);
              } else {
                last_state = name_location::name_proper;
                start_of_name_proper_pos = cur_pos;

                if (is_invalid_character_valid_provided(
                        cur_char, character_constants::g_k_valid_name_chars) ==
                    true) {
                  std::string what_arg{"invalid character in section name"};
                  throw syntax_error::generate_formatted_error(
                      what_arg, ctx.file_path, cur_pos);
                } else {
                  ret_val.first.push_back(cur_char);
                }
              }
            }
          }
        }
      } break;

      case name_location::leading_whitespace: {
        if (eof == true) {
          std::string what_arg{"expected section name"};
          throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                       cur_pos);
        } else {
          if (is_whitespace(cur_char) == true) {
            ;
          } else {
            if (cur_char ==
                character_constants::g_k_section_name_closing_delimiter) {
              last_state = name_location::closing_delimiter;

              std::string what_arg{"empty section names are not permitted"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, cur_pos);
            } else {
              last_state = name_location::name_proper;
              start_of_name_proper_pos = cur_pos;

              if (is_invalid_character_valid_provided(
                      cur_char, character_constants::g_k_valid_name_chars) ==
                  true) {
                std::string what_arg{"invalid character in section name"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path, cur_pos);
              } else {
                ret_val.first.push_back(cur_char);
              }
            }
          }
        }
      } break;

      case name_location::name_proper: {
        if (eof == true) {
          std::string what_arg{"unterminated section name"};
          throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                       cur_pos);
        } else {
          if (is_whitespace(cur_char) == true) {
            last_state = name_location::trailing_whitespace;
          } else {
            if (cur_char ==
                character_constants::g_k_section_name_closing_delimiter) {
              last_state = name_location::closing_delimiter;
            } else {
              if (last_newline_pos > start_of_name_proper_pos) {
                std::string what_arg{
                    "section name must appear completely on one line"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path, cur_pos);
              } else {
                if (is_invalid_character_valid_provided(
                        cur_char, character_constants::g_k_valid_name_chars) ==
                    true) {
                  std::string what_arg{"invalid character in section name"};
                  throw syntax_error::generate_formatted_error(
                      what_arg, ctx.file_path, cur_pos);
                } else {
                  ret_val.first.push_back(cur_char);
                }
              }
            }
          }
        }
      } break;

      case name_location::trailing_whitespace: {
        if (eof == true) {
          std::string what_arg{"unterminated section name"};
          throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                       cur_pos);
        } else {
          if (is_whitespace(cur_char) == true) {
            ;
          } else {
            if (cur_char ==
                character_constants::g_k_section_name_closing_delimiter) {
              last_state = name_location::closing_delimiter;
            } else {
              std::string what_arg{
                  "character after trailing whitespace in section name"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, cur_pos);
            }
          }
        }
      } break;

      case name_location::closing_delimiter: {
        throw std::runtime_error{"impossible!"};
      } break;
      }
    }
  } else {
    ret_val.first = "";
  }

  if (is_root_section == false) {

    enum class name_body_gap_location {
      separating_whitespace,
      opening_body_delimiter,
    };

    for (name_body_gap_location last_state{
             name_body_gap_location::separating_whitespace};
         last_state != name_body_gap_location::opening_body_delimiter;) {
      char cur_char{};
      bool eof{false};
      std::ifstream::pos_type cur_pos{};
      do {
        cur_pos = ctx.file.tellg();
        if (ctx.file.eof() == true) {
          eof = true;
          break;
        } else {
          ctx.file.get(cur_char);
        }
      } while (cur_char == character_constants::g_k_newline);

      switch (last_state) {

      case name_body_gap_location::separating_whitespace: {
        if (eof == true) {
          std::string what_arg{"expected section body"};
          throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                       cur_pos);
        } else {
          if (is_whitespace(cur_char) == true) {
            ;
          } else if (cur_char ==
                     character_constants::g_k_section_body_opening_delimiter) {
            last_state = name_body_gap_location::opening_body_delimiter;
          } else {
            std::string what_arg{"expected section body opening delimiter"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, cur_pos);
          }
        }
      } break;

      case name_body_gap_location::opening_body_delimiter: {
        throw std::runtime_error{"impossible!"};
      } break;
      }
    }
  }

  {
    bool ended_on_body_closing_delimiter{false};
    std::ifstream::pos_type cur_pos{};
    for (;;) {
      char cur_char{};
      bool eof{false};
      do {
        cur_pos = ctx.file.tellg();
        if (ctx.file.eof() == true) {
          eof = true;
          break;
        } else {
          ctx.file.get(cur_char);
        }
      } while (cur_char == character_constants::g_k_newline);

      if (eof == true) {
        break;
      } else {
        if (is_whitespace(cur_char)) {
          ;
        } else if (cur_char ==
                   character_constants::g_k_section_body_closing_delimiter) {
          ended_on_body_closing_delimiter = true;
          break;
        } else if (cur_char ==
                   character_constants::g_k_section_name_opening_delimiter) {
          ctx.file.unget();
          std::ifstream::pos_type start_pos{cur_pos};

          std::pair<std::string, node_ptr<section_node>> new_section{
              parse_section(ctx, false)};

          if (ret_val.second->contains(new_section.first) == true) {
            std::string what_arg{"duplicate name in scope"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, start_pos);
          } else {
            ret_val.second->insert({std::move(new_section)});
          }
        } else if (false) {
          // TODO directives
        } else {
          ctx.file.unget();
          std::ifstream::pos_type start_pos{ctx.file.tellg()};

          std::pair<std::string, node_ptr<value_node>> new_key_value{
              parse_key_value(ctx)};

          if (ret_val.second->contains(new_key_value.first) == true) {
            std::string what_arg{"duplicate name in scope"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, start_pos);
          } else {
            ret_val.second->insert({std::move(new_key_value)});
          }
        }
      }
    }

    if ((ended_on_body_closing_delimiter == false) &&
        (is_root_section == false)) {
      std::string what_arg{"expected section body closing delimiter"};
      throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                   cur_pos);
    }
  }

  return ret_val;
}

std::pair<std::string, libconfigfile::node_ptr<libconfigfile::value_node>>
libconfigfile::parser::impl::parse_key_value(context &ctx) {
  std::pair<std::string, node_ptr<value_node>> ret_val{};

  ret_val.first = parse_key_value_key(ctx);
  ret_val.second = parse_key_value_value(ctx);

  return ret_val;
}

std::string libconfigfile::parser::impl::parse_key_value_key(context &ctx) {
  std::string key_name{};

  enum class key_name_location {
    leading_whitespace,
    name_proper,
    trailing_whitespace,
    equal_sign,
  };

  for (key_name_location last_state{key_name_location::leading_whitespace};
       last_state != key_name_location::equal_sign;) {

    char cur_char{};
    bool eof{false};
    std::ifstream::pos_type cur_pos{};
    do {
      cur_pos = ctx.file.tellg();
      if (ctx.file.eof() == true) {
        eof = true;
        break;
      } else {
        ctx.file.get(cur_char);
      }
    } while (cur_char == character_constants::g_k_newline);

    switch (last_state) {

    case key_name_location::leading_whitespace: {
      if (eof == true) {
        std::string what_arg{"expected key name"};
        throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                     cur_pos);
      } else {
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
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, cur_pos);
            } break;

            case character_constants::g_k_key_value_terminate: {
              std::string what_arg{"empty key-value"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, cur_pos);
            } break;

            default: {
              std::string what_arg{"invalid character in key name"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, cur_pos);
            } break;
            }

          } else {
            key_name.push_back(cur_char);
          }
        }
      }
    } break;

    case key_name_location::name_proper: {
      if (eof == true) {
        std::string what_arg{"missing value part of key-value"};
        throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                     cur_pos);
      } else {
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
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, cur_pos);
            } break;

            default: {
              std::string what_arg{"invalid character in key name"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, cur_pos);
            } break;
            }
          } else {
            key_name.push_back(cur_char);
          }
        }
      }
    } break;

    case key_name_location::trailing_whitespace: {
      if (eof == true) {
        std::string what_arg{"missing value part of key-value"};
        throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                     cur_pos);
      } else {
        if (is_whitespace(cur_char) == true) {
          ;
        } else {
          if (cur_char == character_constants::g_k_key_value_assign) {
            last_state = key_name_location::equal_sign;
          } else {
            if (cur_char == character_constants::g_k_key_value_terminate) {
              std::string what_arg{"missing value part of key-value"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, cur_pos);
            } else {
              std::string what_arg{"expected value assigment after key name"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, cur_pos);
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

  return key_name;
}

libconfigfile::node_ptr<libconfigfile::value_node>
libconfigfile::parser::impl::parse_key_value_value(context &ctx) {
  std::string value_contents{};
  std::ifstream::pos_type value_start_pos{};

  {
    enum class value_location {
      equal_sign,
      leading_whitespace,
      value_proper,
      semicolon
    };

    bool first_loop{true};
    bool in_string{false};
    char last_char{};
    char cur_char{};

    for (value_location last_state{value_location::equal_sign};
         last_state != value_location::semicolon;
         first_loop = false, last_char = cur_char) {

      bool eof{false};
      std::ifstream::pos_type cur_pos{};
      do {
        cur_pos = ctx.file.tellg();
        if (ctx.file.eof() == true) {
          eof = true;
          break;
        } else {
          ctx.file.get(cur_char);
        }
      } while (cur_char == character_constants::g_k_newline);

      switch (last_state) {

      case value_location::equal_sign: {
        if (eof == true) {
          std::string what_arg{"missing value part of key-value"};
          throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                       cur_pos);
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
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, cur_pos);
            } else {
              last_state = value_location::value_proper;
              value_start_pos = cur_pos;
              value_contents.push_back(cur_char);

              if (cur_char == character_constants::g_k_string_delimiter) {
                in_string = !in_string;
              }
            }
          }
        }
      } break;

      case value_location::leading_whitespace: {
        if (eof == true) {
          std::string what_arg{"missing value part of key-value"};
          throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                       cur_pos);
        } else {
          if (is_whitespace(cur_char)) {
            ;
          } else if (cur_char == character_constants::g_k_key_value_terminate) {
            last_state = value_location::semicolon;
            std::string what_arg{"empty value part of key-value"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, cur_pos);
          } else {
            last_state = value_location::value_proper;
            value_start_pos = cur_pos;
            value_contents.push_back(cur_char);

            if (cur_char == character_constants::g_k_string_delimiter) {
              in_string = !in_string;
            }
          }
        }
      } break;

      case value_location::value_proper: {
        if (eof == true) {
          std::string what_arg{"unterminated key-value"};
          throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                       cur_pos);
        } else {
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
        throw std::runtime_error{"impossible!"};
      } break;
      }
    }
  }

  value_contents = trim_whitespace(
      value_contents, character_constants::g_k_whitespace_chars, false, true);

  node_ptr<value_node> ret_val{node_ptr_cast<value_node>(
      call_appropriate_value_parse_func(ctx, value_contents, value_start_pos))};

  return ret_val;
}

libconfigfile::node_ptr<libconfigfile::array_value_node>
libconfigfile::parser::impl::parse_array_value(
    context &ctx, const std::string &raw_value,
    const std::ifstream::pos_type &start_pos) {
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
  char cur_char{};
  char last_char{};

  std::pair<std::string, std::ifstream::pos_type> cur_raw_element{{},
                                                                  start_pos};
  std::vector<std::pair<std::string, std::ifstream::pos_type>> raw_elements{};

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
            what_arg, ctx.file_path,
            (start_pos + static_cast<std::ifstream::off_type>(raw_value_idx)));
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
              what_arg, ctx.file_path,
              (start_pos +
               static_cast<std::ifstream::off_type>(raw_value_idx)));
        } else {
          last_char_type = char_type::element_proper;
          cur_raw_element.second =
              (start_pos + static_cast<std::ifstream::off_type>(raw_value_idx));
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
              what_arg, ctx.file_path,
              (start_pos +
               static_cast<std::ifstream::off_type>(raw_value_idx)));
        } else {
          last_char_type = char_type::element_proper;
          cur_raw_element.second =
              (start_pos + static_cast<std::ifstream::off_type>(raw_value_idx));
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
              what_arg, ctx.file_path,
              (start_pos +
               static_cast<std::ifstream::off_type>(raw_value_idx)));
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
              what_arg, ctx.file_path,
              (start_pos +
               static_cast<std::ifstream::off_type>(raw_value_idx)));
        } else {
          last_char_type = char_type::element_proper;
          cur_raw_element.second =
              (start_pos + static_cast<std::ifstream::off_type>(raw_value_idx));
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
            what_arg, ctx.file_path,
            (start_pos + static_cast<std::ifstream::off_type>(raw_value_idx)));
      }
    } break;

    case char_type::trailing_whitespace: {
      if (is_whitespace(cur_char) == true) {
        ;
      } else {
        std::string what_arg{"extraneous character(s) after array"};
        throw syntax_error::generate_formatted_error(
            what_arg, ctx.file_path,
            (start_pos + static_cast<std::ifstream::off_type>(raw_value_idx)));
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
        what_arg, ctx.file_path,
        (start_pos + static_cast<std::ifstream::off_type>((
                         (raw_value.empty()) ? (0) : (raw_value.size() - 1)))));
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
        ctx, raw_elements[i].first, raw_elements[i].second));
  }

  return ret_val;
}

libconfigfile::node_ptr<libconfigfile::integer_end_value_node>
libconfigfile::parser::impl::parse_integer_value(
    context &ctx, const std::string &raw_value,
    const std::ifstream::pos_type &start_pos) {
  if (raw_value.empty()) {
    std::string what_arg{"empty value"};
    throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                 start_pos);
  } else {
    std::string actual_digits{};
    actual_digits.reserve(raw_value.size());
    bool is_negative{false};
    const numeral_system *num_sys{nullptr};

    bool last_char_was_digit{false};
    bool any_digits_so_far{false};

    bool last_char_was_leading_zero{false};
    size_t num_of_leading_zeroes{0};

    static_assert(character_constants::g_k_num_sys_prefix_leader == '0');

    for (std::string::size_type raw_value_idx{0};
         raw_value_idx < raw_value.size(); ++raw_value_idx) {
      char cur_char{raw_value[raw_value_idx]};

      const auto default_char_behaviour{
          [&ctx, &start_pos, &num_sys, &cur_char, &last_char_was_digit,
           &any_digits_so_far, &last_char_was_leading_zero, &actual_digits,
           &raw_value_idx]() {
            num_sys =
                ((num_sys == nullptr) ? (&numeral_system_decimal) : (num_sys));

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
                  what_arg, ctx.file_path,
                  (start_pos +
                   static_cast<std::ifstream::off_type>(raw_value_idx)));
            }
          }};

      switch (cur_char) {

      case character_constants::g_k_num_digit_separator: {
        if ((last_char_was_digit == false) ||
            (raw_value_idx == (raw_value.size() - 1))) {
          std::string what_arg{"integer digit separator must be surrounded by "
                               "at least one digit on each side"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path,
              (start_pos +
               static_cast<std::ifstream::off_type>(raw_value_idx)));
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
              what_arg, ctx.file_path,
              (start_pos +
               static_cast<std::ifstream::off_type>(raw_value_idx)));
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
              what_arg, ctx.file_path,
              (start_pos +
               static_cast<std::ifstream::off_type>(raw_value_idx)));
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

      case numeral_system_binary.prefix:
      case numeral_system_binary.prefix_alt: {
        if (num_sys == nullptr) {
          if (any_digits_so_far == false) {
            if (last_char_was_leading_zero == true) {
              if (num_of_leading_zeroes == 1) {
                last_char_was_digit = false;
                last_char_was_leading_zero = false;

                num_sys = &numeral_system_binary;
              } else {
                std::string what_arg{"numeral system prefix must appear "
                                     "before integer digits"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path,
                    (start_pos +
                     static_cast<std::ifstream::off_type>(raw_value_idx)));
              }
            } else {
              std::string what_arg{"invalid character in integer"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path,
                  (start_pos +
                   static_cast<std::ifstream::off_type>(raw_value_idx)));
            }
          } else {
            std::string what_arg{
                "numeral system prefix must appear before integer digits"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path,
                (start_pos +
                 static_cast<std::ifstream::off_type>(raw_value_idx)));
          }
        } else {
          default_char_behaviour();
        }
      } break;

      case numeral_system_octal.prefix:
      case numeral_system_octal.prefix_alt: {
        if (num_sys == nullptr) {
          if (any_digits_so_far == false) {
            if (last_char_was_leading_zero == true) {
              if (num_of_leading_zeroes == 1) {
                last_char_was_digit = false;
                last_char_was_leading_zero = false;

                num_sys = &numeral_system_octal;
              } else {
                std::string what_arg{"numeral system prefix must appear "
                                     "before integer digits"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path,
                    (start_pos +
                     static_cast<std::ifstream::off_type>(raw_value_idx)));
              }
            } else {
              std::string what_arg{"invalid character in integer"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path,
                  (start_pos +
                   static_cast<std::ifstream::off_type>(raw_value_idx)));
            }
          } else {
            std::string what_arg{
                "numeral system prefix must appear before integer digits"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path,
                (start_pos +
                 static_cast<std::ifstream::off_type>(raw_value_idx)));
          }
        } else {
          default_char_behaviour();
        }
      } break;

      case numeral_system_hexadecimal.prefix:
      case numeral_system_hexadecimal.prefix_alt: {
        if (num_sys == nullptr) {
          if (any_digits_so_far == false) {
            if (last_char_was_leading_zero == true) {
              if (num_of_leading_zeroes == 1) {
                last_char_was_digit = false;
                last_char_was_leading_zero = false;

                num_sys = &numeral_system_hexadecimal;
              } else {
                std::string what_arg{"numeral system prefix must appear "
                                     "before integer digits"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path,
                    (start_pos +
                     static_cast<std::ifstream::off_type>(raw_value_idx)));
              }
            } else {
              std::string what_arg{"invalid character in integer"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path,
                  (start_pos +
                   static_cast<std::ifstream::off_type>(raw_value_idx)));
            }
          } else {
            std::string what_arg{
                "numeral system prefix must appear before integer digits"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path,
                (start_pos +
                 static_cast<std::ifstream::off_type>(raw_value_idx)));
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
      num_sys = &numeral_system_decimal;
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
            std::stoi(actual_digits, nullptr, num_sys->base), num_sys);
      } else if constexpr ((sizeof(decltype(std::stol("")))) >=
                           (sizeof(integer_end_value_node_data_t))) {
        ret_val = make_node_ptr<integer_end_value_node>(
            std::stol(actual_digits, nullptr, num_sys->base), num_sys);
      } else {
        ret_val = make_node_ptr<integer_end_value_node>(
            std::stoll(actual_digits, nullptr, num_sys->base), num_sys);
      }
    } catch (const std::out_of_range &ex) {
      std::string what_arg{"integer value is too large"};
      throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                   start_pos);
    }

    if (is_negative == true) {
      ret_val->set(-(ret_val->get()));
    }

    return ret_val;
  }
}

libconfigfile::node_ptr<libconfigfile::float_end_value_node>
libconfigfile::parser::impl::parse_float_value(
    context &ctx, const std::string &raw_value,
    const std::ifstream::pos_type &start_pos) {
  if (raw_value.empty()) {
    std::string what_arg{"empty value"};
    throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                 start_pos);
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

      const std::string::size_type last_raw_value_idx{
          ((raw_value.empty()) ? (0) : (raw_value.size() - 1))};

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
                  what_arg, ctx.file_path,
                  (start_pos +
                   static_cast<std::ifstream::off_type>(raw_value_idx)));
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
                  what_arg, ctx.file_path,
                  (start_pos +
                   static_cast<std::ifstream::off_type>(raw_value_idx)));
            }
          } break;

          case character_constants::g_k_num_digit_separator: {
            if (last_char == char_type::digit) {
              if (raw_value_idx != last_raw_value_idx) {
                char next_char{raw_value[raw_value_idx + 1]};
                if (is_digit(next_char, numeral_system_decimal) == true) {
                  last_char = char_type::separator;
                } else {
                  std::string what_arg{
                      "float digit separator must be surrounded by "
                      "at least one digit on each side"};
                  throw syntax_error::generate_formatted_error(
                      what_arg, ctx.file_path,
                      (start_pos +
                       static_cast<std::ifstream::off_type>(raw_value_idx)));
                }
              } else {
                std::string what_arg{
                    "float digit separator must be surrounded by "
                    "at least one digit on each side"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path,
                    (start_pos +
                     static_cast<std::ifstream::off_type>(raw_value_idx)));
              }
            } else {
              std::string what_arg{
                  "float digit separator must be surrounded by "
                  "at least one digit on each side"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path,
                  (start_pos +
                   static_cast<std::ifstream::off_type>(raw_value_idx)));
            }
          } break;

          case character_constants::g_k_float_decimal_point: {
            if (last_char == char_type::digit) {
              if (raw_value_idx != last_raw_value_idx) {
                char next_char{raw_value[raw_value_idx + 1]};
                if (is_digit(next_char, numeral_system_decimal) == true) {
                  last_char = char_type::decimal;
                  cur_location = num_location::fractional;
                  sanitized_string.push_back(cur_char);
                } else {
                  std::string what_arg{
                      "float decimal point must be surrounded by at "
                      "least one digit on each side"};
                  throw syntax_error::generate_formatted_error(
                      what_arg, ctx.file_path,
                      (start_pos +
                       static_cast<std::ifstream::off_type>(raw_value_idx)));
                }
              } else {
                std::string what_arg{
                    "float decimal point must be surrounded by at "
                    "least one digit on each side"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path,
                    (start_pos +
                     static_cast<std::ifstream::off_type>(raw_value_idx)));
              }
            } else {
              std::string what_arg{
                  "float decimal point must be surrounded by at "
                  "least one digit on each side"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path,
                  (start_pos +
                   static_cast<std::ifstream::off_type>(raw_value_idx)));
            }
          } break;

          case character_constants::g_k_float_exponent_sign_lower:
          case character_constants::g_k_float_exponent_sign_upper: {
            if (last_char == char_type::digit) {
              if (raw_value_idx != last_raw_value_idx) {
                char next_char{raw_value[raw_value_idx + 1]};
                if ((is_digit(next_char, numeral_system_decimal) == true) ||
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
                      what_arg, ctx.file_path,
                      (start_pos +
                       static_cast<std::ifstream::off_type>(raw_value_idx)));
                }
              } else {
                std::string what_arg{
                    "float exponent sign must be surrounded by at "
                    "least on digit on each side"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path,
                    (start_pos +
                     static_cast<std::ifstream::off_type>(raw_value_idx)));
              }
            } else {
              std::string what_arg{
                  "float exponent sign must be surrounded by at "
                  "least on digit on each side"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path,
                  (start_pos +
                   static_cast<std::ifstream::off_type>(raw_value_idx)));
            }
          } break;

          default: {
            if (is_digit(cur_char, numeral_system_decimal) == true) {
              last_char = char_type::digit;
              sanitized_string.push_back(cur_char);
            } else {
              std::string what_arg{"invalid character in float"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path,
                  (start_pos +
                   static_cast<std::ifstream::off_type>(raw_value_idx)));
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
                what_arg, ctx.file_path,
                (start_pos +
                 static_cast<std::ifstream::off_type>(raw_value_idx)));
          } break;

          case character_constants::g_k_num_negative_sign: {
            std::string what_arg{"negative sign may only appear at start of "
                                 "integer part or exponent part of float"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path,
                (start_pos +
                 static_cast<std::ifstream::off_type>(raw_value_idx)));
          } break;

          case character_constants::g_k_num_digit_separator: {
            if (last_char == char_type::digit) {
              if (raw_value_idx != last_raw_value_idx) {
                char next_char{raw_value[raw_value_idx + 1]};
                if (is_digit(next_char, numeral_system_decimal) == true) {
                  last_char = char_type::separator;
                } else {
                  std::string what_arg{
                      "float digit separator must be surrounded by "
                      "at least one digit on each side"};
                  throw syntax_error::generate_formatted_error(
                      what_arg, ctx.file_path,
                      (start_pos +
                       static_cast<std::ifstream::off_type>(raw_value_idx)));
                }
              } else {
                std::string what_arg{
                    "float digit separator must be surrounded by "
                    "at least one digit on each side"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path,
                    (start_pos +
                     static_cast<std::ifstream::off_type>(raw_value_idx)));
              }
            } else {
              std::string what_arg{
                  "float digit separator must be surrounded by "
                  "at least one digit on each side"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path,
                  (start_pos +
                   static_cast<std::ifstream::off_type>(raw_value_idx)));
            }
          } break;

          case character_constants::g_k_float_decimal_point: {
            std::string what_arg{"floats can only contain one decimal point"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path,
                (start_pos +
                 static_cast<std::ifstream::off_type>(raw_value_idx)));
          } break;

          case character_constants::g_k_float_exponent_sign_lower:
          case character_constants::g_k_float_exponent_sign_upper: {
            if (last_char == char_type::digit) {
              if (raw_value_idx != last_raw_value_idx) {
                char next_char{raw_value[raw_value_idx + 1]};
                if ((is_digit(next_char, numeral_system_decimal) == true) ||
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
                      what_arg, ctx.file_path,
                      (start_pos +
                       static_cast<std::ifstream::off_type>(raw_value_idx)));
                }
              } else {
                std::string what_arg{
                    "float exponent sign must be surrounded by at "
                    "least on digit on each side"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path,
                    (start_pos +
                     static_cast<std::ifstream::off_type>(raw_value_idx)));
              }
            } else {
              std::string what_arg{
                  "float exponent sign must be surrounded by at "
                  "least on digit on each side"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path,
                  (start_pos +
                   static_cast<std::ifstream::off_type>(raw_value_idx)));
            }
          } break;

          default: {
            if (is_digit(cur_char, numeral_system_decimal) == true) {
              last_char = char_type::digit;
              sanitized_string.push_back(cur_char);
            } else {
              std::string what_arg{"invalid character in float"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path,
                  (start_pos +
                   static_cast<std::ifstream::off_type>(raw_value_idx)));
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
                  what_arg, ctx.file_path,
                  (start_pos +
                   static_cast<std::ifstream::off_type>(raw_value_idx)));
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
                  what_arg, ctx.file_path,
                  (start_pos +
                   static_cast<std::ifstream::off_type>(raw_value_idx)));
            }
          } break;

          case character_constants::g_k_num_digit_separator: {
            if (last_char == char_type::digit) {
              if (raw_value_idx != last_raw_value_idx) {
                char next_char{raw_value[raw_value_idx + 1]};
                if (is_digit(next_char, numeral_system_decimal) == true) {
                  last_char = char_type::separator;
                } else {
                  std::string what_arg{
                      "float digit separator must be surrounded by "
                      "at least one digit on each side"};
                  throw syntax_error::generate_formatted_error(
                      what_arg, ctx.file_path,
                      (start_pos +
                       static_cast<std::ifstream::off_type>(raw_value_idx)));
                }
              } else {
                std::string what_arg{
                    "float digit separator must be surrounded by "
                    "at least one digit on each side"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path,
                    (start_pos +
                     static_cast<std::ifstream::off_type>(raw_value_idx)));
              }
            } else {
              std::string what_arg{
                  "float digit separator must be surrounded by "
                  "at least one digit on each side"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path,
                  (start_pos +
                   static_cast<std::ifstream::off_type>(raw_value_idx)));
            }
          } break;

          case character_constants::g_k_float_decimal_point: {
            std::string what_arg{
                "float decimal point can not appear after exponent sign"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path,
                (start_pos +
                 static_cast<std::ifstream::off_type>(raw_value_idx)));
          } break;

          case character_constants::g_k_float_exponent_sign_lower:
          case character_constants::g_k_float_exponent_sign_upper: {
            std::string what_arg{"float exponent sign can only appear once"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path,
                (start_pos +
                 static_cast<std::ifstream::off_type>(raw_value_idx)));
          } break;

          default: {
            if (is_digit(cur_char, numeral_system_decimal) == true) {
              last_char = char_type::digit;
              sanitized_string.push_back(cur_char);
            } else {
              std::string what_arg{"invalid character in float"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path,
                  (start_pos +
                   static_cast<std::ifstream::off_type>(raw_value_idx)));
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
        throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                     start_pos);
      }

      return ret_val;
    }
  }
}

libconfigfile::node_ptr<libconfigfile::string_end_value_node>
libconfigfile::parser::impl::parse_string_value(
    context &ctx, const std::string &raw_value,
    const std::ifstream::pos_type &start_pos) {
  if (raw_value.empty()) {
    std::string what_arg{"empty value"};
    throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                 start_pos);
  } else {
    bool in_string{false};

    std::string string_contents{};
    string_contents.reserve(raw_value.size());

    for (std::string::size_type raw_value_idx{};
         raw_value_idx < raw_value.size(); ++raw_value_idx) {
      char cur_char{raw_value[raw_value_idx]};
      if (in_string == true) {
        if (cur_char == character_constants::g_k_string_delimiter) {
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

                if ((is_digit(hex_digit_1, numeral_system_hexadecimal)) &&
                    (is_digit(hex_digit_2, numeral_system_hexadecimal))) {
                  std::string hex_string{std::string{} + hex_digit_1 +
                                         hex_digit_2};
                  string_contents.push_back(static_cast<char>(std::stoi(
                      hex_string, nullptr, numeral_system_hexadecimal.base)));
                  raw_value_idx = hex_digit_2;
                } else {
                  std::string what_arg{
                      "invalid digit in hexadecimal escape sequence"};
                  throw syntax_error::generate_formatted_error(
                      what_arg, ctx.file_path,
                      (start_pos +
                       static_cast<std::ifstream::off_type>(raw_value_idx)));
                }
              } else {
                std::string what_arg{"incomplete escape sequence in string"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path,
                    (start_pos +
                     static_cast<std::ifstream::off_type>(raw_value_idx)));
              }
            } else {
              if (character_constants::g_k_basic_escape_chars.contains(
                      escape_char)) {
                string_contents.push_back(
                    character_constants::g_k_basic_escape_chars.at(
                        escape_char));
                raw_value_idx = escape_char_pos;
              }
            }
          } else {
            std::string what_arg{"incomplete escape sequence in string"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path,
                (start_pos +
                 static_cast<std::ifstream::off_type>(raw_value_idx)));
          }
        } else {
          string_contents.push_back(cur_char);
        }
      } else {
        if (is_whitespace(cur_char,
                          character_constants::g_k_whitespace_chars) == true) {
        } else if (cur_char == character_constants::g_k_string_delimiter) {
          in_string = true;
        } else {
          std::string what_arg{"invalid character outside of string"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path,
              (start_pos +
               static_cast<std::ifstream::off_type>(raw_value_idx)));
        }
      }
    }

    if (in_string == true) {
      std::string what_arg{"unterminated string"};
      throw syntax_error::generate_formatted_error(
          what_arg, ctx.file_path,
          (start_pos +
           static_cast<std::ifstream::off_type>(
               ((raw_value.empty()) ? (0) : (raw_value.size() - 1)))));
    } else {
      return make_node_ptr<string_end_value_node>(string_contents);
    }
  }
}

libconfigfile::node_ptr<libconfigfile::value_node>
libconfigfile::parser::impl::call_appropriate_value_parse_func(
    context &ctx, const std::string &raw_value,
    const std::ifstream::pos_type &start_pos) {

  std::variant<value_node_type, end_value_node_type> value_type_variant{
      identify_key_value_value_type(raw_value)};

  switch (value_type_variant.index()) {

  case 0: {
    value_node_type value_type_extracted{std::get<0>(value_type_variant)};

    switch (value_type_extracted) {

    case value_node_type::ARRAY: {
      return node_ptr_cast<value_node>(
          parse_array_value(ctx, raw_value, start_pos));
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
          parse_string_value(ctx, raw_value, start_pos));
    } break;

    case end_value_node_type::INTEGER: {
      return node_ptr_cast<value_node>(
          parse_integer_value(ctx, raw_value, start_pos));
    } break;

    case end_value_node_type::FLOAT: {
      return node_ptr_cast<value_node>(
          parse_float_value(ctx, raw_value, start_pos));
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

std::pair<libconfigfile::parser::impl::directive,
          std::optional<libconfigfile::node_ptr<libconfigfile::section_node>>>
libconfigfile::parser::impl::parse_directive(context &ctx) {
  std::ifstream::pos_type start_pos{ctx.file.tellg()};

  std::string name{};
  name.reserve(character_constants::g_k_max_directive_name_length);

  enum class name_location {
    directive_leader,
    leading_whitespace,
    name_proper,
    done,
  };

  std::ifstream::pos_type cur_pos{};

  for (name_location last_state{name_location::directive_leader};
       last_state != name_location::done;) {
    char cur_char{};
    bool eof{false};
    std::ifstream::pos_type last_newline_pos{};
    while (true) {
      cur_pos = ctx.file.tellg();
      if (ctx.file.eof() == true) {
        eof = true;
        break;
      } else {
        ctx.file.get(cur_char);
        if (cur_char == character_constants::g_k_newline) {
          last_newline_pos = cur_pos;
          continue;
        } else {
          break;
        }
      }
    }

    switch (last_state) {

    case name_location::directive_leader: {
      if (eof == true) {
        std::string what_arg{"expected directive name"};
        throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                     cur_pos);
      } else {
        if (cur_char == character_constants::g_k_directive_leader) {
          ;
        } else if (is_whitespace(cur_char,
                                 character_constants::g_k_whitespace_chars) ==
                   true) {
          last_state = name_location::leading_whitespace;
          ;
        } else {
          if (last_newline_pos > start_pos) {
            std::string what_arg{"entire directive must appear on one line"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, cur_pos);
          } else {
            last_state = name_location::name_proper;
            name.push_back(cur_char);
          }
        }
      }
    } break;

    case name_location::leading_whitespace: {
      if (eof == true) {
        std::string what_arg{"expected directive name"};
        throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                     cur_pos);
      } else {
        if (is_whitespace(cur_char,
                          character_constants::g_k_whitespace_chars) == true) {
          ;
        } else {
          if (last_newline_pos > start_pos) {
            std::string what_arg{"entire directive must appear on one line"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, cur_pos);
          } else {
            last_state = name_location::name_proper;
            name.push_back(cur_char);
          }
        }
      }
    } break;

    case name_location::name_proper: {
      if (eof == true) {
        last_state = name_location::done;
      } else {
        if (is_whitespace(cur_char,
                          character_constants::g_k_whitespace_chars) == true) {
          last_state = name_location::done;
        } else {
          if (last_newline_pos > start_pos) {
            std::string what_arg{"entire directive must appear on one line"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, cur_pos);
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

  directive directive_func_to_call{directive::null};

  if (name == character_constants::g_k_version_directive_name) {
    directive_func_to_call = directive::version;
  } else if (name == character_constants::g_k_include_directive_name) {
    directive_func_to_call = directive::include;
  } else {
    std::string what_arg{"invalid directive"};
    throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                 cur_pos);
  }

  for (;;) {
    char cur_char{};
    bool eof{false};
    do {
      cur_pos = ctx.file.tellg();
      if (ctx.file.eof() == true) {
        eof = true;
        break;
      } else {
        ctx.file.get(cur_char);
      }
    } while (cur_char == character_constants::g_k_newline);

    if (eof == true) {
      break;
    } else {
      if (is_whitespace(cur_char, character_constants::g_k_whitespace_chars) ==
          true) {
        ;
      } else {
        break;
      }
    }
  }

  switch (directive_func_to_call) {
  case directive::version: {
    return {directive::version, std::nullopt};
  } break;
  case directive::include: {
    return {directive::include, parse_include_directive(ctx)};
  } break;
  default: {
    throw std::runtime_error{"impossible!"};
  } break;
  }
}

void libconfigfile::parser::impl::parse_version_directive(context &ctx) {
  std::ifstream::pos_type start_pos{ctx.file.tellg()};

  std::string version_str{};

  enum class args_location {
    leading_whitespace,
    opening_delimiter,
    version_str,
    closing_delimiter,
    trailing_whitespace,
    done,
  };

  std::ifstream::pos_type cur_pos{};
  std::ifstream::pos_type start_of_version_str_pos{};

  for (args_location last_state{args_location::leading_whitespace};
       last_state != args_location::done;) {
    char cur_char{};
    bool eof{false};
    std::ifstream::pos_type last_newline_pos{};
    while (true) {
      cur_pos = ctx.file.tellg();
      if (ctx.file.eof() == true) {
        eof = true;
        break;
      } else {
        ctx.file.get(cur_char);
        if (cur_char == character_constants::g_k_newline) {
          last_newline_pos = cur_pos;
          continue;
        } else {
          break;
        }
      }
    }

    switch (last_state) {

    case args_location::leading_whitespace: {
      if (eof == true) {
        std::string what_arg{"version directive requires version argument"};
        throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                     cur_pos);
      } else {
        if (is_whitespace(cur_char,
                          character_constants::g_k_whitespace_chars) == true) {
          ;
        } else if (cur_char == character_constants::g_k_string_delimiter) {
          if (last_newline_pos > start_pos) {
            std::string what_arg{"entire directive must appear on one line"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, cur_pos);
          } else {
            last_state = args_location::opening_delimiter;
          }
        } else {
          std::string what_arg{"version directive requires version argument"};
          throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                       cur_pos);
        }
      }
    } break;

    case args_location::opening_delimiter: {
      if (eof == true) {
        std::string what_arg{
            "unterminated string in version directive argument"};
        throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                     cur_pos);
      } else {
        if (last_newline_pos > start_pos) {
          std::string what_arg{"entire directive must appear on one line"};
          throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                       cur_pos);
        } else {
          if (cur_char == character_constants::g_k_string_delimiter) {
            last_state = args_location::closing_delimiter;
            start_of_version_str_pos = cur_pos;
          } else {
            version_str.push_back(cur_char);
            last_state = args_location::version_str;
            start_of_version_str_pos = cur_pos;
          }
        }
      }
    } break;

    case args_location::version_str: {
      if (eof == true) {
        std::string what_arg{
            "unterminated string in version directive argument"};
        throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                     cur_pos);
      } else {
        if (last_newline_pos > start_pos) {
          std::string what_arg{"entire directive must appear on one line"};
          throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                       cur_pos);
        } else {
          if (cur_char == character_constants::g_k_string_delimiter) {
            last_state = args_location::closing_delimiter;
          } else {
            version_str.push_back(cur_char);
          }
        }
      }
    } break;

    case args_location::closing_delimiter: {
      if (eof == true) {
        last_state = args_location::done;
      } else {
        if (last_newline_pos > start_pos) {
          last_state = args_location::done;
        } else {
          if (is_whitespace(cur_char,
                            character_constants::g_k_whitespace_chars) ==
              true) {
            last_state = args_location::trailing_whitespace;
          } else {
            std::string what_arg{"excess arguments given to version directive"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, cur_pos);
          }
        }
      }
    } break;

    case args_location::trailing_whitespace: {
      if (eof == true) {
        last_state = args_location::done;
      } else {
        if (last_newline_pos > start_pos) {
          last_state = args_location::done;
        } else {
          if (is_whitespace(cur_char,
                            character_constants::g_k_whitespace_chars) ==
              true) {
            ;
          } else {
            std::string what_arg{"excess arguments given to version directive"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, cur_pos);
          }
        }
      }
    } break;

    case args_location::done: {
      throw std::runtime_error{"impossible!"};
    } break;
    }
  }

  if (version_str.empty() == true) {
    std::string what_arg{"empty version argument given to version directive"};
    throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                 cur_pos);
  } else {
    if (version_str == g_k_version) {
      return;
    } else {
      std::string what_arg{
          "incompatible parser and configuration file versions"};
      throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                   start_of_version_str_pos);
    }
  }
}

libconfigfile::node_ptr<libconfigfile::section_node>
libconfigfile::parser::impl::parse_include_directive(context &ctx) {
  std::ifstream::pos_type start_pos{ctx.file.tellg()};

  std::string file_path{};

  enum class args_location {
    leading_whitespace,
    opening_delimiter,
    file_path,
    closing_delimiter,
    trailing_whitespace,
    done,
  };

  std::ifstream::pos_type cur_pos{};
  std::ifstream::pos_type start_of_file_path_pos{};

  bool last_char_was_escape_leader{false};

  for (args_location last_state{args_location::leading_whitespace};
       last_state != args_location::done;) {

    char cur_char{};
    bool eof{false};
    std::ifstream::pos_type last_newline_pos{};
    while (true) {
      cur_pos = ctx.file.tellg();
      if (ctx.file.eof() == true) {
        eof = true;
        break;
      } else {
        ctx.file.get(cur_char);
        if (cur_char == character_constants::g_k_newline) {
          last_newline_pos = cur_pos;
          continue;
        } else {
          break;
        }
      }
    }

    switch (last_state) {

    case args_location::leading_whitespace: {
      if (eof == true) {
        std::string what_arg{"include directive requires file path argument"};
        throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                     cur_pos);
      } else {
        if (is_whitespace(cur_char,
                          character_constants::g_k_whitespace_chars) == true) {
          ;
        } else if (cur_char == character_constants::g_k_string_delimiter) {
          if (last_newline_pos > start_pos) {
            std::string what_arg{"entire directive must appear on one line"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, cur_pos);
          } else {
            last_state = args_location::opening_delimiter;
          }
        } else {
          std::string what_arg{"include directive requires file path argument"};
          throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                       cur_pos);
        }
      }
    } break;

    case args_location::opening_delimiter: {
      if (eof == true) {
        std::string what_arg{
            "unterminated string in include directive argument"};
        throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                     cur_pos);
      } else {
        if (last_newline_pos > start_pos) {
          std::string what_arg{"entire directive must appear on one line"};
          throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                       cur_pos);
        } else {
          if (cur_char == character_constants::g_k_string_delimiter) {
            last_state = args_location::closing_delimiter;
            start_of_file_path_pos = cur_pos;
          } else if (cur_char == character_constants::g_k_escape_leader) {
            last_char_was_escape_leader = true;
            file_path.push_back(cur_char);
            last_state = args_location::file_path;
            start_of_file_path_pos = cur_pos;
          } else {
            file_path.push_back(cur_char);
            last_state = args_location::file_path;
            start_of_file_path_pos = cur_pos;
          }
        }
      }
    } break;

    case args_location::file_path: {
      if (eof == true) {
        std::string what_arg{
            "unterminated string in include directive argument"};
        throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                     cur_pos);
      } else {
        if (last_newline_pos > start_pos) {
          std::string what_arg{"entire directive must appear on one line"};
          throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                       cur_pos);
        } else {
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
      if (eof == true) {
        last_state = args_location::done;
      } else {
        if (last_newline_pos > start_pos) {
          last_state = args_location::done;
        } else {
          if (is_whitespace(cur_char,
                            character_constants::g_k_whitespace_chars) ==
              true) {
            last_state = args_location::trailing_whitespace;
          } else {
            std::string what_arg{"excess arguments given to include directive"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, cur_pos);
          }
        }
      }
    } break;

    case args_location::trailing_whitespace: {
      if (eof == true) {
        last_state = args_location::done;
      } else {
        if (last_newline_pos > start_pos) {
          last_state = args_location::done;
        } else {
          if (is_whitespace(cur_char,
                            character_constants::g_k_whitespace_chars) ==
              true) {
            ;
          } else {
            std::string what_arg{"excess arguments given to include directive"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, cur_pos);
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
    throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                 cur_pos);
  } else {
    std::variant<std::string, std::string::size_type> file_path_escaped{
        replace_escape_sequences(file_path)};

    switch (file_path_escaped.index()) {
    case 0: {
      file_path = std::get<std::string>(std::move(file_path_escaped));
      return parser::parse(file_path);
    } break;

    case 1: {
      std::ifstream::pos_type invalid_escape_sequence_pos{
          start_of_file_path_pos};
      invalid_escape_sequence_pos +=
          std::ifstream::off_type{static_cast<std::ifstream::off_type>(
              std::get<std::string::size_type>(file_path_escaped))};

      std::string what_arg{
          "invalid escape sequence in include directive argument"};
      throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                   invalid_escape_sequence_pos);
    } break;

    default: {
      throw std::runtime_error{"impossible!"};
    } break;
    }
  }
}

void libconfigfile::parser::impl::handle_comments(context &ctx) {
  static_assert(character_constants::g_k_comment_cpp.front() ==
                character_constants::g_k_comment_c_start.front());
  static_assert(character_constants::g_k_comment_cpp.size() == 2);
  static_assert(character_constants::g_k_comment_c_start.size() == 2);
  static_assert(character_constants::g_k_comment_c_end.size() == 2);

  static constexpr char c_or_cpp_comment_leader{
      character_constants::g_k_comment_cpp.front()};

  if (ctx.file.eof() == true) {
    return;
  } else {
    char cur_char{};
    ctx.file.get(cur_char);

    if (cur_char == character_constants::g_k_comment_script) {
      while (true) {
        if (ctx.file.eof() == true) {
          return;
        } else if (cur_char == character_constants::g_k_newline) {
          return;
        } else {
          ctx.file.get(cur_char);
        }
      }
    } else if (cur_char == c_or_cpp_comment_leader) {
      char next_char{};
      ctx.file.get(next_char);

      if (next_char == character_constants::g_k_comment_cpp.back()) {
        while (true) {
          if (ctx.file.eof() == true) {
            return;
          } else if (next_char == character_constants::g_k_newline) {
            return;
          } else {
            ctx.file.get(next_char);
          }
        }
      } else if (next_char == character_constants::g_k_comment_c_start.back()) {
        while (true) {
          if (ctx.file.eof() == true) {
            return;
          } else if (next_char ==
                     character_constants::g_k_comment_c_end.front()) {
            char next_next_char{};
            ctx.file.get(next_next_char);
            if (next_next_char ==
                character_constants::g_k_comment_c_end.back()) {
              return;
            } else {
              next_char = next_next_char;
            }
          } else {
            ctx.file.get(next_char);
          }
        }
      } else {
        ctx.file.putback(next_char);
        ctx.file.putback(cur_char);
        return;
      }
    } else {
      ctx.file.putback(cur_char);
      return;
    }
  }
}

std::variant<libconfigfile::value_node_type, libconfigfile::end_value_node_type>
libconfigfile::parser::impl::identify_key_value_value_type(
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
libconfigfile::parser::impl::identify_key_value_numeric_value_type(
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
        if (((value_contents.find(numeral_system_hexadecimal.prefix)) !=
             (std::string::npos)) ||
            ((value_contents.find(numeral_system_hexadecimal.prefix_alt)) !=
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
libconfigfile::parser::impl::replace_escape_sequences(const std::string &str) {
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
            if ((is_digit(hex_digit_1, numeral_system_hexadecimal)) &&
                (is_digit(hex_digit_2, numeral_system_hexadecimal))) {
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
libconfigfile::parser::impl::extract_strings(
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

std::string libconfigfile::parser::impl::get_substr_between_indices_inclusive(
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

std::string libconfigfile::parser::impl::get_substr_between_indices_exclusive(
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

bool libconfigfile::parser::impl::is_whitespace(
    const char ch,
    const std::string
        &whitespace_chars /*= character_constants::g_k_whitespace_chars*/) {
  return ((whitespace_chars.find(ch)) != (std::string::npos));
}

std::string libconfigfile::parser::impl::trim_whitespace(
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

bool libconfigfile::parser::impl::is_actual_delimiter(
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

bool libconfigfile::parser::impl::is_invalid_character_valid_provided(
    const char ch, const std::string &valid_chars) {
  return (valid_chars.find(ch) == std::string::npos);
}

bool libconfigfile::parser::impl::is_invalid_character_invalid_provided(
    const char ch, const std::string &invalid_chars) {
  return (invalid_chars.find(ch) != std::string::npos);
}

std::pair<bool, std::string::size_type>
libconfigfile::parser::impl::contains_invalid_character_valid_provided(
    const std::string &str, const std::string &valid_chars) {
  for (size_t i{0}; i < str.size(); ++i) {
    if (is_invalid_character_valid_provided(str[i], valid_chars) == true) {
      return {true, i};
    }
  }
  return {false, std::string::npos};
}

std::pair<bool, std::string::size_type>
libconfigfile::parser::impl::contains_invalid_character_invalid_provided(
    const std::string &str, const std::string &invalid_chars) {
  for (size_t i{0}; i < str.size(); ++i) {
    if (is_invalid_character_invalid_provided(str[i], invalid_chars) == true) {
      return {true, i};
    }
  }
  return {false, std::string::npos};
}

bool libconfigfile::parser::impl::is_digit(
    char ch,
    const numeral_system &num_sys /*= character_constants::g_k_dec_num_sys*/) {
  return (num_sys.digits.find(ch) != std::string::npos);
}

bool libconfigfile::parser::impl::case_insensitive_string_compare(
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

std::string::size_type
libconfigfile::parser::impl::case_insensitive_string_find(
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

bool libconfigfile::parser::impl::string_contains_only(
    const std::string &str, const std::string &chars) {
  return ((str.find_first_not_of(chars)) == (std::string::npos));
}

bool libconfigfile::parser::impl::string_contains_any_of(
    const std::string &str, const std::string &chars) {
  return ((str.find_first_of(chars)) != (std::string::npos));
}

std::string
libconfigfile::parser::impl::string_to_upper(const std::string &str) {
  std::string ret_val{};
  ret_val.resize(str.size());

  for (size_t i{0}; i < str.size(); ++i) {
    ret_val[i] = std::toupper(str[i]);
  }

  return ret_val;
}

std::string
libconfigfile::parser::impl::string_to_lower(const std::string &str) {
  std::string ret_val{};
  ret_val.resize(str.size());

  for (size_t i{0}; i < str.size(); ++i) {
    ret_val[i] = std::tolower(str[i]);
  }

  return ret_val;
}
