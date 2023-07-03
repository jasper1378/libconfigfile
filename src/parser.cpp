#include "parser.hpp"

#include "array_value_node.hpp"
#include "character_constants.hpp"
#include "constexpr_tolower_toupper.hpp"
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
  context ctx{file_path, std::ifstream{file_path}, 1, 0,
              make_node_ptr<section_node>()};

  if ((ctx.file_stream.is_open() == false) ||
      (ctx.file_stream.good() == false)) {
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
    std::ifstream::int_type first_char{ctx.file_stream.peek()};
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

    std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
        start_of_name_proper_pos_count{};

    bool first_loop{true};

    for (name_location last_state{name_location::opening_delimiter};
         last_state != name_location::closing_delimiter; first_loop = false) {

      char cur_char{};
      bool eof{false};
      bool handled_comment_in_name_proper{};
      std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
          pos_count_before_handled_comment_in_name_proper{};
      while (true) {
        if (last_state == name_location::name_proper) {
          pos_count_before_handled_comment_in_name_proper = {ctx.line_count,
                                                             ctx.char_count};
          handled_comment_in_name_proper = handle_comments(ctx);
        } else {
          handle_comments(ctx);
        }
        ctx.file_stream.get(cur_char);
        if (ctx.file_stream.eof() == true) {
          eof = true;
          break;
        } else if (cur_char == character_constants::g_k_newline) {
          ++ctx.line_count;
          ctx.char_count = 0;
          continue;
        } else {
          ++ctx.char_count;
          break;
        }
      }

      switch (last_state) {
      case name_location::opening_delimiter: {
        if (eof == true) {
          std::string what_arg{"expected section name"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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
                    what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
              } else {
                last_state = name_location::name_proper;
                start_of_name_proper_pos_count = {ctx.line_count,
                                                  ctx.char_count};

                if (is_invalid_character_valid_provided(
                        cur_char, character_constants::g_k_valid_name_chars) ==
                    true) {
                  std::string what_arg{"invalid character in section name"};
                  throw syntax_error::generate_formatted_error(
                      what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } else {
          if (is_whitespace(cur_char) == true) {
            ;
          } else {
            if (cur_char ==
                character_constants::g_k_section_name_closing_delimiter) {
              last_state = name_location::closing_delimiter;

              std::string what_arg{"empty section names are not permitted"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
            } else {
              last_state = name_location::name_proper;
              start_of_name_proper_pos_count = {ctx.line_count, ctx.char_count};

              if (is_invalid_character_valid_provided(
                      cur_char, character_constants::g_k_valid_name_chars) ==
                  true) {
                std::string what_arg{"invalid character in section name"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } else {
          if (is_whitespace(cur_char) == true) {
            last_state = name_location::trailing_whitespace;
          } else {
            if (cur_char ==
                character_constants::g_k_section_name_closing_delimiter) {
              last_state = name_location::closing_delimiter;
            } else {
              if (start_of_name_proper_pos_count.first != ctx.line_count) {
                std::string what_arg{
                    "section name must appear completely on one line"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
              } else if (handled_comment_in_name_proper == true) {
                std::string what_arg{
                    "section name can not be split by comments"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path,
                    pos_count_before_handled_comment_in_name_proper.first,
                    pos_count_before_handled_comment_in_name_proper.second);
              } else {
                if (is_invalid_character_valid_provided(
                        cur_char, character_constants::g_k_valid_name_chars) ==
                    true) {
                  std::string what_arg{"invalid character in section name"};
                  throw syntax_error::generate_formatted_error(
                      what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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
                  what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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
      while (true) {
        handle_comments(ctx);
        ctx.file_stream.get(cur_char);
        if (ctx.file_stream.eof() == true) {
          eof = true;
          break;
        } else if (cur_char == character_constants::g_k_newline) {
          ++ctx.line_count;
          ctx.char_count = 0;
          continue;
        } else {
          ++ctx.char_count;
          break;
        }
      }

      switch (last_state) {

      case name_body_gap_location::separating_whitespace: {
        if (eof == true) {
          std::string what_arg{"expected section body"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } else {
          if (is_whitespace(cur_char) == true) {
            ;
          } else if (cur_char ==
                     character_constants::g_k_section_body_opening_delimiter) {
            last_state = name_body_gap_location::opening_body_delimiter;
          } else {
            std::string what_arg{"expected section body opening delimiter"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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
    for (;;) {

      char cur_char{};
      bool eof{false};
      while (true) {
        handle_comments(ctx);
        ctx.file_stream.get(cur_char);
        if (ctx.file_stream.eof() == true) {
          eof = true;
          break;
        } else if (cur_char == character_constants::g_k_newline) {
          ++ctx.line_count;
          ctx.char_count = 0;
          continue;
        } else {
          ++ctx.char_count;
          break;
        }
      }

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
          ctx.file_stream.unget();
          std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
              start_pos_count{ctx.line_count, ctx.char_count};

          std::pair<std::string, node_ptr<section_node>> new_section{
              parse_section(ctx, false)};

          if (ret_val.second->contains(new_section.first) == true) {
            std::string what_arg{"duplicate name in scope"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, start_pos_count.first,
                start_pos_count.second);
          } else {
            ret_val.second->insert({std::move(new_section)});
          }
        } else if (cur_char == character_constants::g_k_directive_leader) {
          std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
              start_pos_count{ctx.line_count, ctx.char_count};
          std::pair<directive, std::optional<node_ptr<section_node>>> dir_res{
              parse_directive(ctx)};

          switch (dir_res.first) {
          case directive::null: {
            throw std::runtime_error{"impossible!"};
          } break;

          case directive::version: {
            ;
          } break;

          case directive::include: {
            assert(dir_res.second);
            for (auto i{dir_res.second.value()->begin()};
                 i != dir_res.second.value()->end(); ++i) {
              if (ret_val.second->contains(i->first)) {
                std::string what_arg{"duplicate name in scope"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path, start_pos_count.first,
                    start_pos_count.second);
              }
            }

            ret_val.second->insert(
                std::make_move_iterator(dir_res.second.value()->begin()),
                std::make_move_iterator(dir_res.second.value()->end()));
          } break;
          }
        } else {
          ctx.file_stream.unget();
          std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
              start_pos_count{ctx.line_count, ctx.char_count};

          std::pair<std::string, node_ptr<value_node>> new_key_value{
              parse_key_value(ctx)};

          if (ret_val.second->contains(new_key_value.first) == true) {
            std::string what_arg{"duplicate name in scope"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, start_pos_count.first,
                start_pos_count.second);
          } else {
            ret_val.second->insert({std::move(new_key_value)});
          }
        }
      }
    }

    if ((ended_on_body_closing_delimiter == false) &&
        (is_root_section == false)) {
      std::string what_arg{"expected section body closing delimiter"};
      throw syntax_error::generate_formatted_error(
          what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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

  std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
      start_of_name_proper_pos_count{};

  for (key_name_location last_state{key_name_location::leading_whitespace};
       last_state != key_name_location::equal_sign;) {

    char cur_char{};
    bool eof{false};
    bool handled_comment_in_name_proper{};
    std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
        pos_count_before_handled_comment_in_name_proper{};
    while (true) {
      if (last_state == key_name_location::name_proper) {
        pos_count_before_handled_comment_in_name_proper = {ctx.line_count,
                                                           ctx.char_count};
        handled_comment_in_name_proper = handle_comments(ctx);
      } else {
        handle_comments(ctx);
      }
      ctx.file_stream.get(cur_char);
      if (ctx.file_stream.eof() == true) {
        eof = true;
        break;
      } else if (cur_char == character_constants::g_k_newline) {
        ++ctx.line_count;
        ctx.char_count = 0;
        continue;
      } else {
        ++ctx.char_count;
        break;
      }
    }

    switch (last_state) {

    case key_name_location::leading_whitespace: {
      if (eof == true) {
        std::string what_arg{"expected key name"};
        throw syntax_error::generate_formatted_error(
            what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
      } else {
        if (is_whitespace(cur_char)) {
          ;
        } else {
          start_of_name_proper_pos_count = {ctx.line_count, ctx.char_count};
          last_state = key_name_location::name_proper;

          if (is_invalid_character_valid_provided(
                  cur_char, character_constants::g_k_valid_name_chars) ==
              true) {
            switch (cur_char) {

            case character_constants::g_k_key_value_assign: {
              std::string what_arg{"missing key-value name"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
            } break;

            case character_constants::g_k_key_value_terminate: {
              std::string what_arg{"empty key-value"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
            } break;

            default: {
              std::string what_arg{"invalid character in key name"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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
        throw syntax_error::generate_formatted_error(
            what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
      } else {
        if (is_whitespace(cur_char) == true) {
          last_state = key_name_location::trailing_whitespace;
        } else {
          if (cur_char == character_constants::g_k_key_value_assign) {
            last_state = key_name_location::equal_sign;
          } else if (cur_char == character_constants::g_k_key_value_terminate) {
            std::string what_arg{"missing value part of key-value"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
          } else {
            if (start_of_name_proper_pos_count.first != ctx.line_count) {
              std::string what_arg{
                  "key name must appear completely on one line"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
            } else if (handled_comment_in_name_proper == true) {
              std::string what_arg{"key name can not be split by comments"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path,
                  pos_count_before_handled_comment_in_name_proper.first,
                  pos_count_before_handled_comment_in_name_proper.second);
            } else {
              if (is_invalid_character_valid_provided(
                      cur_char, character_constants::g_k_valid_name_chars) ==
                  true) {
                std::string what_arg{"invalid character in key name"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
              } else {
                key_name.push_back(cur_char);
              }
            }
          }
        }
      }
    } break;

    case key_name_location::trailing_whitespace: {
      if (eof == true) {
        std::string what_arg{"missing value part of key-value"};
        throw syntax_error::generate_formatted_error(
            what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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
                  what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
            } else {
              std::string what_arg{"expected value assigment after key name"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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
  bool first_loop{true};
  char cur_char{};

  for (; true; first_loop = false) {

    bool eof{false};
    while (true) {
      handle_comments(ctx);
      ctx.file_stream.get(cur_char);
      if (ctx.file_stream.eof() == true) {
        eof = true;
        break;
      } else if (cur_char == character_constants::g_k_newline) {
        ++ctx.line_count;
        ctx.char_count = 0;
        continue;
      } else {
        ++ctx.char_count;
        break;
      }
    }

    if (eof == true) {
      std::string what_arg{"expected value part of key-value"};
      throw syntax_error::generate_formatted_error(
          what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
    } else if (is_whitespace(cur_char) == true) {
      continue;
    } else if ((cur_char == character_constants::g_k_key_value_assign) &&
               (first_loop == true)) {
      continue;
    } else if (cur_char == character_constants::g_k_key_value_terminate) {
      std::string what_arg{"expected value part of key-value"};
      throw syntax_error::generate_formatted_error(
          what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
    } else {
      ctx.file_stream.unget();
      --ctx.char_count;
      return call_appropriate_value_parse_func(
          ctx, std::string{std::string{} +
                           character_constants::g_k_key_value_terminate});
    }
  }
}

libconfigfile::node_ptr<libconfigfile::array_value_node>
libconfigfile::parser::impl::parse_array_value(
    context &ctx, const std::string &possible_terminating_chars,
    char *actual_terminating_char /*= nullptr*/) {
  node_ptr<libconfigfile::array_value_node> ret_val{
      make_node_ptr<array_value_node>()};

  static const std::string possible_terminating_chars_for_elements{
      std::string{} + character_constants::g_k_array_element_separator +
      character_constants::g_k_array_closing_delimiter};

  enum class char_type {
    leading_whitespace,
    opening_delimiter,
    element_separator,
    closing_delimiter,
  };

  char_type last_char_type{char_type::leading_whitespace};

  char cur_char{};

  for (;;) {
    bool eof{false};
    while (true) {
      handle_comments(ctx);
      ctx.file_stream.get(cur_char);
      if (ctx.file_stream.eof() == true) {
        eof = true;
        break;
      } else if (cur_char == character_constants::g_k_newline) {
        ++ctx.line_count;
        ctx.char_count = 0;
        continue;
      } else {
        ++ctx.char_count;
        break;
      }
    }

    if (eof == true) {
      std::string what_arg{"unterminated array key-value"};
      throw syntax_error::generate_formatted_error(
          what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
    } else if ((possible_terminating_chars.find(cur_char) !=
                std::string::npos) &&
               (last_char_type == char_type::closing_delimiter)) {
      if (actual_terminating_char != nullptr) {
        *actual_terminating_char = cur_char;
      };
      break;
    } else if (is_whitespace(cur_char)) {
      continue;
    } else {
      switch (last_char_type) {

      case char_type::leading_whitespace: {
        if (cur_char == character_constants::g_k_array_opening_delimiter) {
          last_char_type = char_type::opening_delimiter;
        } else {
          std::string what_arg{"expected array opening delimiter"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        }
      } break;

      case char_type::opening_delimiter: {
        if (cur_char == character_constants::g_k_array_closing_delimiter) {
          last_char_type = char_type::closing_delimiter;
        } else if (cur_char ==
                   character_constants::g_k_array_element_separator) {
          std::string what_arg{"expected array element before separator"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } else {
          --ctx.char_count;
          ctx.file_stream.unget();
          char actual_terminating_char{};
          ret_val->push_back(call_appropriate_value_parse_func(
              ctx, possible_terminating_chars_for_elements,
              &actual_terminating_char));
          switch (actual_terminating_char) {
          case character_constants::g_k_array_element_separator: {
            last_char_type = char_type::element_separator;
          } break;
          case character_constants::g_k_array_closing_delimiter: {
            last_char_type = char_type::closing_delimiter;
          } break;
          }
        }
      } break;

      case char_type::element_separator: {
        if (cur_char == character_constants::g_k_array_closing_delimiter) {
          last_char_type = char_type::closing_delimiter;
        } else if (cur_char ==
                   character_constants::g_k_array_element_separator) {
          std::string what_arg{"expected array element or closing delimiter"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } else {
          --ctx.char_count;
          ctx.file_stream.unget();
          char actual_terminating_char{};
          ret_val->push_back(call_appropriate_value_parse_func(
              ctx, possible_terminating_chars_for_elements,
              &actual_terminating_char));
          switch (actual_terminating_char) {
          case character_constants::g_k_array_element_separator: {
            last_char_type = char_type::element_separator;
          } break;
          case character_constants::g_k_array_closing_delimiter: {
            last_char_type = char_type::closing_delimiter;
          } break;
          }
        }
      } break;

      case char_type::closing_delimiter: {
        std::string what_arg{"extraneous character(s) after array"};
        throw syntax_error::generate_formatted_error(
            what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
      } break;
      }
    }
  }
  return ret_val;
}

libconfigfile::node_ptr<libconfigfile::integer_end_value_node>
libconfigfile::parser::impl::parse_integer_value(
    context &ctx, const std::string &possible_terminating_chars,
    char *actual_terminating_char /*= nullptr*/) {
  static_assert(character_constants::g_k_num_sys_prefix_leader == '0');

  std::string actual_digits{};
  bool is_negative{false};
  const numeral_system *num_sys{nullptr};

  std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
      pos_count_at_start{ctx.line_count, ctx.char_count};

  bool last_char_was_digit{false};
  bool any_digits_so_far{false};
  bool last_char_was_leading_zero{false};
  size_t num_of_leading_zeroes{0};

  char cur_char{};

  const auto default_char_behavior{
      [&ctx, &num_sys, &cur_char, &last_char_was_digit, &any_digits_so_far,
       &last_char_was_leading_zero, &actual_digits]() {
        if (num_sys == nullptr) {
          num_sys = &numeral_system_decimal;
        }

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
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        }
      }};

  bool first_loop{true};
  for (;; first_loop = false) {
    bool eof{false};
    bool handled_comment{};
    std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
        pos_count_before_handled_comment{};
    while (true) {
      pos_count_before_handled_comment = {ctx.line_count, ctx.char_count};
      handled_comment = handle_comments(ctx);
      ctx.file_stream.get(cur_char);
      if (ctx.file_stream.eof() == true) {
        eof = true;
        break;
      } else if (cur_char == character_constants::g_k_newline) {
        ++ctx.line_count;
        ctx.char_count = 0;
        continue;
      } else {
        ++ctx.char_count;
        break;
      }
    }

    if (eof == true) {
      std::string what_arg{"unterminated integer key-value"};
      throw syntax_error::generate_formatted_error(
          what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
    } else if (possible_terminating_chars.find(cur_char) != std::string::npos) {
      if (actual_terminating_char != nullptr) {
        *actual_terminating_char = cur_char;
      };
      break;
    } else if (pos_count_at_start.first != ctx.line_count) {
      std::string what_arg{"integer must appear completely on one line"};
      throw syntax_error::generate_formatted_error(
          what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
    } else if (handled_comment == true) {
      std::string what_arg{"integer can not be split by comments"};
      throw syntax_error::generate_formatted_error(
          what_arg, ctx.file_path, pos_count_before_handled_comment.first,
          pos_count_before_handled_comment.second);
    } else {

      switch (cur_char) {
      case character_constants::g_k_num_digit_separator: {
        if ((last_char_was_digit == false) ||
            (ctx.file_stream.peek() == std::ifstream::traits_type::eof())) {
          std::string what_arg{"integer digit separator must be surrounded by "
                               "at least one digit on each side"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } else {
          last_char_was_digit = false;
          last_char_was_leading_zero = false;
        }
      } break;

      case character_constants::g_k_num_positive_sign: {
        if (first_loop == true) {
          is_negative = false;
          last_char_was_digit = false;
          last_char_was_leading_zero = false;
        } else {
          std::string what_arg{"positive sign must appear at start of integer"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        }
      } break;

      case character_constants::g_k_num_negative_sign: {
        if (first_loop == true) {
          is_negative = true;
          last_char_was_digit = false;
          last_char_was_leading_zero = false;
        } else {
          std::string what_arg{"negative sign must appear at start of integer"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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
                std::string what_arg{
                    "numeral system prefix must appear before integer digits"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
              }
            } else {
              std::string what_arg{"invalid character in integer"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
            }
          } else {
            std::string what_arg{
                "numeral system prefix must appear before integer digits"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
          }
        } else {
          default_char_behavior();
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
                std::string what_arg{
                    "numeral system prefix must appear before integer digits"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
              }
            } else {
              std::string what_arg{"invalid character in integer"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
            }
          } else {
            std::string what_arg{
                "numeral system prefix must appear before integer digits"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
          }
        } else {
          default_char_behavior();
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
                std::string what_arg{
                    "numeral system prefix must appear before integer digits"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
              }
            } else {
              std::string what_arg{"invalid character in integer"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
            }
          } else {
            std::string what_arg{
                "numeral system prefix must appear before integer digits"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
          }
        } else {
          default_char_behavior();
        }
      } break;

      default: {
        default_char_behavior();
      } break;
      }
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
                "std::stol(), std::stoll()) with return type large enough for "
                "integer_end_value_node_t");

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
                                                 pos_count_at_start.first,
                                                 pos_count_at_start.second);
  }

  if (is_negative == true) {
    ret_val->set(-(ret_val->get()));
  }

  return ret_val;
}

libconfigfile::node_ptr<libconfigfile::float_end_value_node>
libconfigfile::parser::impl::parse_float_value(
    context &ctx, const std::string &possible_terminating_chars,
    char *actual_terminating_char /*= nullptr*/) {
  std::string sanitized_string{};

  std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
      pos_count_at_start{ctx.line_count, ctx.char_count};

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

  for (;;) {
    char cur_char{};
    bool eof{false};
    bool handled_comment{};
    std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
        pos_count_before_handled_comment{};
    while (true) {
      pos_count_before_handled_comment = {ctx.line_count, ctx.char_count};
      handled_comment = handle_comments(ctx);
      ctx.file_stream.get(cur_char);
      if (ctx.file_stream.eof() == true) {
        eof = true;
        break;
      } else if (cur_char == character_constants::g_k_newline) {
        ++ctx.line_count;
        ctx.char_count = 0;
        continue;
      } else {
        ++ctx.char_count;
        break;
      }
    }

    if (eof == true) {
      std::string what_arg{"unterminated integer key-value"};
      throw syntax_error::generate_formatted_error(
          what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
    } else if (possible_terminating_chars.find(cur_char) != std::string::npos) {
      if (actual_terminating_char != nullptr) {
        *actual_terminating_char = cur_char;
      };
      break;
    } else if (pos_count_at_start.first != ctx.line_count) {
      std::string what_arg{"float must appear completely on one line"};
      throw syntax_error::generate_formatted_error(
          what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
    } else if (handled_comment == true) {
      std::string what_arg{"float can not be split by comments"};
      throw syntax_error::generate_formatted_error(
          what_arg, ctx.file_path, pos_count_before_handled_comment.first,
          pos_count_before_handled_comment.second);
    } else {

      switch (cur_location) {

      case num_location::integer: {
        switch (cur_char) {

        case libconfigfile::tolower<
            character_constants::g_k_float_infinity.second.front()>():
        case libconfigfile::toupper<
            character_constants::g_k_float_infinity.second.front()>(): {
          std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
              pos_count_at_start{ctx.line_count, ctx.char_count};

          if ((last_char == char_type::start) ||
              (last_char == char_type::positive) ||
              (last_char == char_type::negative)) {
            std::string special_float_string{};
            special_float_string.reserve(
                character_constants::g_k_float_infinity.second.size());
            special_float_string.push_back(cur_char);

            while (true) {
              while (true) {
                handle_comments(ctx);
                ctx.file_stream.get(cur_char);
                if (ctx.file_stream.eof() == true) {
                  eof = true;
                  break;
                } else if (cur_char == character_constants::g_k_newline) {
                  ++ctx.line_count;
                  ctx.char_count = 0;
                  continue;
                } else {
                  ++ctx.char_count;
                  break;
                }
              }
              if (eof == true) {
                std::string what_arg{"unterminated integer key-value"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
              } else if (possible_terminating_chars.find(cur_char) !=
                         std::string::npos) {
                if (actual_terminating_char != nullptr) {
                  *actual_terminating_char = cur_char;
                };
                break;
              } else {
                special_float_string.push_back(cur_char);
              }
            }

            if (case_insensitive_string_compare(
                    special_float_string,
                    character_constants::g_k_float_infinity.second) == true) {
              node_ptr<float_end_value_node> ret_val{nullptr};

              if (last_char == char_type::negative) {
                ret_val = make_node_ptr<float_end_value_node>(
                    -(character_constants::g_k_float_infinity.first));
              } else {
                ret_val = make_node_ptr<float_end_value_node>(
                    character_constants::g_k_float_infinity.first);
              }

              return ret_val;
            } else {
              std::string what_arg{"invalid character in float"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, pos_count_at_start.first,
                  pos_count_at_start.second);
            }
          } else {
            std::string what_arg{"invalid character in float"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, pos_count_at_start.first,
                pos_count_at_start.second);
          }
        } break;

        case libconfigfile::tolower<
            character_constants::g_k_float_not_a_number.second.front()>():
        case libconfigfile::toupper<
            character_constants::g_k_float_not_a_number.second.front()>(): {
          std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
              pos_count_at_start{ctx.line_count, ctx.char_count};

          if ((last_char == char_type::start) ||
              (last_char == char_type::positive) ||
              (last_char == char_type::negative)) {
            std::string special_float_string{};
            special_float_string.reserve(
                character_constants::g_k_float_not_a_number.second.size());
            special_float_string.push_back(cur_char);

            while (true) {
              while (true) {
                handle_comments(ctx);
                ctx.file_stream.get(cur_char);
                if (ctx.file_stream.eof() == true) {
                  eof = true;
                  break;
                } else if (cur_char == character_constants::g_k_newline) {
                  ++ctx.line_count;
                  ctx.char_count = 0;
                  continue;
                } else {
                  ++ctx.char_count;
                  break;
                }
              }
              if (eof == true) {
                std::string what_arg{"unterminated float key-value"};
                throw syntax_error::generate_formatted_error(
                    what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
              } else if (possible_terminating_chars.find(cur_char) !=
                         std::string::npos) {
                if (actual_terminating_char != nullptr) {
                  *actual_terminating_char = cur_char;
                };
                break;
              } else {
                special_float_string.push_back(cur_char);
              }
            }

            if (case_insensitive_string_compare(
                    special_float_string,
                    character_constants::g_k_float_not_a_number.second) ==
                true) {
              node_ptr<float_end_value_node> ret_val{nullptr};

              if (last_char == char_type::negative) {
                ret_val = make_node_ptr<float_end_value_node>(
                    -(character_constants::g_k_float_not_a_number.first));
              } else {
                ret_val = make_node_ptr<float_end_value_node>(
                    character_constants::g_k_float_not_a_number.first);
              }

              return ret_val;
            } else {
              std::string what_arg{"invalid character in float"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, pos_count_at_start.first,
                  pos_count_at_start.second);
            }
          } else {
            std::string what_arg{"invalid character in float"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, pos_count_at_start.first,
                pos_count_at_start.second);
          }
        } break;

        case character_constants::g_k_num_positive_sign: {
          if (last_char == char_type::start) {
            last_char = char_type::positive;
            sanitized_string.push_back(cur_char);
          } else {
            std::string what_arg{"positive sign may only appear at start of "
                                 "integer part or exponent part of float"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
          }
        } break;

        case character_constants::g_k_num_negative_sign: {
          if (last_char == char_type::start) {
            last_char = char_type::negative;
            sanitized_string.push_back(cur_char);
          } else {
            std::string what_arg{"negative sign may only appear at start of "
                                 "integer part or exponent part of float"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
          }
        } break;

        case character_constants::g_k_num_digit_separator: {
          if (last_char == char_type::digit) {
            if (is_digit(static_cast<char>(ctx.file_stream.peek()),
                         numeral_system_decimal) == true) {
              last_char = char_type::separator;
            } else {
              std::string what_arg{
                  "float digit separator must be surrounded by "
                  "at least one digit on each side"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
            }
          } else {
            std::string what_arg{"float digit separator must be surrounded by "
                                 "at least one digit on each side"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
          }
        } break;

        case character_constants::g_k_float_decimal_point: {
          if (last_char == char_type::digit) {
            if (is_digit(static_cast<char>(ctx.file_stream.peek()),
                         numeral_system_decimal) == true) {
              last_char = char_type::decimal;
              cur_location = num_location::fractional;
              sanitized_string.push_back(cur_char);
            } else {
              std::string what_arg{
                  "float decimal point must be surrounded by at "
                  "least one digit on each side"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
            }
          } else {
            std::string what_arg{"float decimal point must be surrounded by at "
                                 "least one digit on each side"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
          }
        } break;

        case character_constants::g_k_float_exponent_sign_lower:
        case character_constants::g_k_float_exponent_sign_upper: {
          if (last_char == char_type::digit) {
            char next_char{static_cast<char>(ctx.file_stream.peek())};
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
                  what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
            }
          } else {
            std::string what_arg{"float exponent sign must be surrounded by at "
                                 "least on digit on each side"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
          }
        } break;

        default: {
          if (is_digit(cur_char, numeral_system_decimal) == true) {
            last_char = char_type::digit;
            sanitized_string.push_back(cur_char);
          } else {
            std::string what_arg{"invalid character in float"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } break;

        case character_constants::g_k_num_negative_sign: {
          std::string what_arg{"negative sign may only appear at start of "
                               "integer part or exponent part of float"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } break;

        case character_constants::g_k_num_digit_separator: {
          if (last_char == char_type::digit) {
            if (is_digit(static_cast<char>(ctx.file_stream.peek()),
                         numeral_system_decimal) == true) {
              last_char = char_type::separator;
            } else {
              std::string what_arg{
                  "float digit separator must be surrounded by "
                  "at least one digit on each side"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
            }
          } else {
            std::string what_arg{"float digit separator must be surrounded by "
                                 "at least one digit on each side"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
          }
        } break;

        case character_constants::g_k_float_decimal_point: {
          std::string what_arg{"floats can only contain one decimal point"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } break;

        case character_constants::g_k_float_exponent_sign_lower:
        case character_constants::g_k_float_exponent_sign_upper: {
          if (last_char == char_type::digit) {
            char next_char{static_cast<char>(ctx.file_stream.peek())};
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
                  what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
            }
          } else {
            std::string what_arg{"float exponent sign must be surrounded by at "
                                 "least on digit on each side"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
          }
        } break;

        default: {
          if (is_digit(cur_char, numeral_system_decimal) == true) {
            last_char = char_type::digit;
            sanitized_string.push_back(cur_char);
          } else {
            std::string what_arg{"invalid character in float"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
          }
        } break;

        case character_constants::g_k_num_digit_separator: {
          if (last_char == char_type::digit) {
            if (is_digit(static_cast<char>(ctx.file_stream.peek()),
                         numeral_system_decimal) == true) {
              last_char = char_type::separator;
            } else {
              std::string what_arg{
                  "float digit separator must be surrounded by "
                  "at least one digit on each side"};
              throw syntax_error::generate_formatted_error(
                  what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
            }
          } else {
            std::string what_arg{"float digit separator must be surrounded by "
                                 "at least one digit on each side"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
          }
        } break;

        case character_constants::g_k_float_decimal_point: {
          std::string what_arg{
              "float decimal point can not appear after exponent sign"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } break;

        case character_constants::g_k_float_exponent_sign_lower:
        case character_constants::g_k_float_exponent_sign_upper: {
          std::string what_arg{"float exponent sign can only appear once"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } break;

        default: {
          if (is_digit(cur_char, numeral_system_decimal) == true) {
            last_char = char_type::digit;
            sanitized_string.push_back(cur_char);
          } else {
            std::string what_arg{"invalid character in float"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
          }
        } break;
        }
      } break;
      }
    }
  }

  if (sanitized_string.empty()) {
    sanitized_string = "0";
  }

  node_ptr<float_end_value_node> ret_val{nullptr};

  static_assert(
      (sizeof(decltype(std::stold(""))) >= sizeof(float_end_value_node_data_t)),
      "no string-to-float conversion function with return type "
      "large enough for float_end_value_node_t");

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
    throw syntax_error::generate_formatted_error(
        what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
  }

  return ret_val;
}

libconfigfile::node_ptr<libconfigfile::string_end_value_node>
libconfigfile::parser::impl::parse_string_value(
    context &ctx, const std::string &possible_terminating_chars,
    char *actual_terminating_char /*= nullptr*/) {
  bool in_string{false};

  std::string string_contents{};

  std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
      last_opening_delimiter_pos_count{};

  for (;;) {
    char cur_char{};
    bool eof{false};
    while (true) {
      if (in_string == false) {
        handle_comments(ctx);
      }
      ctx.file_stream.get(cur_char);
      if (ctx.file_stream.eof() == true) {
        eof = true;
        break;
      } else if (cur_char == character_constants::g_k_newline) {
        ++ctx.line_count;
        ctx.char_count = 0;
        continue;
      } else {
        ++ctx.char_count;
        break;
      }
    }

    if (eof == true) {
      std::string what_arg{"unterminated string key-value"};
      throw syntax_error::generate_formatted_error(
          what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
    } else {
      if (in_string == true) {
        if (last_opening_delimiter_pos_count.first != ctx.line_count) {
          std::string what_arg{"string must appear completely on one line"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } else {
          if (cur_char == character_constants::g_k_string_delimiter) {
            in_string = false;
          } else if (cur_char == character_constants::g_k_escape_leader) {
            ctx.file_stream.unget();
            --ctx.char_count;
            string_contents.push_back(handle_escape_sequence(ctx));
          } else {
            string_contents.push_back(cur_char);
          }
        }
      } else {
        if (possible_terminating_chars.find(cur_char) != std::string::npos) {
          if (actual_terminating_char != nullptr) {
            *actual_terminating_char = cur_char;
          };
          break;
        } else if (is_whitespace(cur_char,
                                 character_constants::g_k_whitespace_chars) ==
                   true) {
        } else if (cur_char == character_constants::g_k_string_delimiter) {
          in_string = true;
          last_opening_delimiter_pos_count = {ctx.line_count, ctx.char_count};
        } else {
          std::string what_arg{"invalid character outside of string"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        }
      }
    }
  }

  if (in_string == true) {
    std::string what_arg{"unterminated string"};
    throw syntax_error::generate_formatted_error(
        what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
  } else {
    return make_node_ptr<string_end_value_node>(std::move(string_contents));
  }
}

libconfigfile::node_ptr<libconfigfile::value_node>
libconfigfile::parser::impl::call_appropriate_value_parse_func(
    context &ctx, const std::string &possible_terminating_chars,
    char *actual_terminating_char /*= nullptr*/) {

  std::variant<value_node_type, end_value_node_type> value_type_variant{
      identify_key_value_value_type(ctx, possible_terminating_chars,
                                    actual_terminating_char)};

  switch (value_type_variant.index()) {

  case 0: {
    value_node_type value_type_extracted{std::get<0>(value_type_variant)};

    switch (value_type_extracted) {

    case value_node_type::ARRAY: {
      return node_ptr_cast<value_node>(parse_array_value(
          ctx, possible_terminating_chars, actual_terminating_char));
    } break;

    default: {
      throw std::runtime_error{"invalid value type returned by "
                               "identify_key_value_value_type()"};
    } break;
    }
  } break;

  case 1: {
    end_value_node_type value_type_extracted{std::get<1>(value_type_variant)};

    switch (value_type_extracted) {

    case end_value_node_type::STRING: {
      return node_ptr_cast<value_node>(parse_string_value(
          ctx, possible_terminating_chars, actual_terminating_char));
    } break;

    case end_value_node_type::INTEGER: {
      return node_ptr_cast<value_node>(parse_integer_value(
          ctx, possible_terminating_chars, actual_terminating_char));
    } break;

    case end_value_node_type::FLOAT: {
      return node_ptr_cast<value_node>(parse_float_value(
          ctx, possible_terminating_chars, actual_terminating_char));
    } break;

    default: {
      throw std::runtime_error{"impossible!"};
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
  std::ifstream::pos_type start_pos_count{ctx.file_stream.tellg()};

  std::string name{};
  name.reserve(character_constants::g_k_max_directive_name_length);

  enum class name_location {
    directive_leader,
    leading_whitespace,
    name_proper,
    done,
  };

  for (name_location last_state{name_location::directive_leader};
       last_state != name_location::done;) {

    char cur_char{};
    bool eof{false};
    std::ifstream::pos_type last_newline_pos_count{};
    while (true) {
      handle_comments(ctx);
      ctx.file_stream.get(cur_char);
      if (ctx.file_stream.eof() == true) {
        eof = true;
        break;
      } else if (cur_char == character_constants::g_k_newline) {
        ++ctx.line_count;
        ctx.char_count = 0;
        continue;
      } else {
        ++ctx.char_count;
        break;
      }
    }

    switch (last_state) {

    case name_location::directive_leader: {
      if (eof == true) {
        std::string what_arg{"expected directive name"};
        throw syntax_error::generate_formatted_error(
            what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
      } else {
        if (cur_char == character_constants::g_k_directive_leader) {
          ;
        } else if (is_whitespace(cur_char,
                                 character_constants::g_k_whitespace_chars) ==
                   true) {
          last_state = name_location::leading_whitespace;
          ;
        } else {
          if (last_newline_pos_count > start_pos_count) {
            std::string what_arg{"entire directive must appear "
                                 "on one line"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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
        throw syntax_error::generate_formatted_error(
            what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
      } else {
        if (is_whitespace(cur_char,
                          character_constants::g_k_whitespace_chars) == true) {
          ;
        } else {
          if (last_newline_pos_count > start_pos_count) {
            std::string what_arg{"entire directive must appear "
                                 "on one line"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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
          if (last_newline_pos_count > start_pos_count) {
            std::string what_arg{"entire directive must appear "
                                 "on one line"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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
    throw syntax_error::generate_formatted_error(
        what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
  }

  switch (directive_func_to_call) {
  case directive::version: {
    parse_version_directive(ctx);
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
  std::pair<decltype(ctx.line_count), decltype(ctx.char_count)> start_pos_count{
      ctx.line_count, ctx.char_count};

  std::string version_str{};

  enum class args_location {
    leading_whitespace,
    opening_delimiter,
    version_str,
    closing_delimiter,
    trailing_whitespace,
    done,
  };

  std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
      start_of_version_str_pos_count{};

  for (args_location last_state{args_location::leading_whitespace};
       last_state != args_location::done;) {

    char cur_char{};
    bool eof{false};
    while (true) {
      if ((last_state != args_location::opening_delimiter) &&
          (last_state != args_location::version_str)) {
        handle_comments(ctx);
      }
      ctx.file_stream.get(cur_char);
      if (ctx.file_stream.eof() == true) {
        eof = true;
        break;
      } else if (cur_char == character_constants::g_k_newline) {
        ++ctx.line_count;
        ctx.char_count = 0;
        continue;
      } else {
        ++ctx.char_count;
        break;
      }
    }
    switch (last_state) {

    case args_location::leading_whitespace: {
      if (eof == true) {
        std::string what_arg{"version directive requires "
                             "version argument"};
        throw syntax_error::generate_formatted_error(
            what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
      } else {
        if (is_whitespace(cur_char,
                          character_constants::g_k_whitespace_chars) == true) {
          ;
        } else if (cur_char == character_constants::g_k_string_delimiter) {
          if (start_pos_count.first != ctx.line_count) {
            std::string what_arg{"entire directive must appear "
                                 "on one line"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
          } else {
            last_state = args_location::opening_delimiter;
          }
        } else {
          std::string what_arg{"version directive requires "
                               "version argument"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        }
      }
    } break;

    case args_location::opening_delimiter: {
      if (eof == true) {
        std::string what_arg{"unterminated string in version "
                             "directive argument"};
        throw syntax_error::generate_formatted_error(
            what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
      } else {
        if (start_pos_count.first != ctx.line_count) {
          std::string what_arg{"entire directive must appear on "
                               "one line"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } else {
          if (cur_char == character_constants::g_k_string_delimiter) {
            last_state = args_location::closing_delimiter;
            start_of_version_str_pos_count = {ctx.line_count, ctx.char_count};
          } else {
            version_str.push_back(cur_char);
            last_state = args_location::version_str;
            start_of_version_str_pos_count = {ctx.line_count, ctx.char_count};
          }
        }
      }
    } break;

    case args_location::version_str: {
      if (eof == true) {
        std::string what_arg{"unterminated string in version "
                             "directive argument"};
        throw syntax_error::generate_formatted_error(
            what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
      } else {
        if (start_pos_count.first != ctx.line_count) {
          std::string what_arg{"entire directive must appear on "
                               "one line"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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
        if (start_pos_count.first != ctx.line_count) {
          last_state = args_location::done;
          ctx.file_stream.unget();
        } else {
          if (is_whitespace(cur_char,
                            character_constants::g_k_whitespace_chars) ==
              true) {
            last_state = args_location::trailing_whitespace;
          } else {
            std::string what_arg{"excess arguments given to "
                                 "version directive"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
          }
        }
      }
    } break;

    case args_location::trailing_whitespace: {
      if (eof == true) {
        last_state = args_location::done;
      } else {
        if (start_pos_count.first != ctx.line_count) {
          last_state = args_location::done;
          ctx.file_stream.unget();
        } else {
          if (is_whitespace(cur_char,
                            character_constants::g_k_whitespace_chars) ==
              true) {
            ;
          } else {
            std::string what_arg{"excess arguments given to "
                                 "version directive"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
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
    std::string what_arg{"empty version argument given to "
                         "version directive"};
    throw syntax_error::generate_formatted_error(
        what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
  } else {
    if (version_str == g_k_version) {
      return;
    } else {
      std::string what_arg{"incompatible parser and "
                           "configuration file versions"};
      throw syntax_error::generate_formatted_error(
          what_arg, ctx.file_path, start_of_version_str_pos_count.first,
          start_of_version_str_pos_count.second);
    }
  }
}

libconfigfile::node_ptr<libconfigfile::section_node>
libconfigfile::parser::impl::parse_include_directive(context &ctx) {
  std::pair<decltype(ctx.line_count), decltype(ctx.char_count)> start_pos_count{
      ctx.line_count, ctx.char_count};

  std::string file_path_str{};

  enum class args_location {
    leading_whitespace,
    opening_delimiter,
    file_path,
    closing_delimiter,
    trailing_whitespace,
    done,
  };

  std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
      start_of_file_path_str_pos_count{};

  bool last_char_was_escape_leader{false};

  for (args_location last_state{args_location::leading_whitespace};
       last_state != args_location::done;) {

    char cur_char{};
    bool eof{false};
    while (true) {
      if ((last_state != args_location::opening_delimiter) &&
          (last_state != args_location::file_path)) {
        handle_comments(ctx);
      }
      ctx.file_stream.get(cur_char);
      if (ctx.file_stream.eof() == true) {
        eof = true;
        break;
      } else if (cur_char == character_constants::g_k_newline) {
        ++ctx.line_count;
        ctx.char_count = 0;
        continue;
      } else {
        ++ctx.char_count;
        break;
      }
    }

    switch (last_state) {

    case args_location::leading_whitespace: {
      if (eof == true) {
        std::string what_arg{"include directive requires file "
                             "path argument"};
        throw syntax_error::generate_formatted_error(
            what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
      } else {
        if (is_whitespace(cur_char,
                          character_constants::g_k_whitespace_chars) == true) {
          ;
        } else if (cur_char == character_constants::g_k_string_delimiter) {
          if (start_pos_count.first != ctx.line_count) {
            std::string what_arg{"entire directive must appear "
                                 "on one line"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
          } else {
            last_state = args_location::opening_delimiter;
          }
        } else {
          std::string what_arg{"include directive requires file "
                               "path argument"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        }
      }
    } break;

    case args_location::opening_delimiter: {
      if (eof == true) {
        std::string what_arg{"unterminated string in include "
                             "directive argument"};
        throw syntax_error::generate_formatted_error(
            what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
      } else {
        if (start_pos_count.first != ctx.line_count) {
          std::string what_arg{"entire directive must appear on "
                               "one line"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } else {
          if (cur_char == character_constants::g_k_string_delimiter) {
            last_state = args_location::closing_delimiter;
            start_of_file_path_str_pos_count = {ctx.line_count, ctx.char_count};
          } else if (cur_char == character_constants::g_k_escape_leader) {
            last_char_was_escape_leader = true;
            file_path_str.push_back(cur_char);
            last_state = args_location::file_path;
            start_of_file_path_str_pos_count = {ctx.line_count, ctx.char_count};
          } else {
            file_path_str.push_back(cur_char);
            last_state = args_location::file_path;
            start_of_file_path_str_pos_count = {ctx.line_count, ctx.char_count};
          }
        }
      }
    } break;

    case args_location::file_path: {
      if (eof == true) {
        std::string what_arg{"unterminated string in include "
                             "directive argument"};
        throw syntax_error::generate_formatted_error(
            what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
      } else {
        if (start_pos_count.first != ctx.line_count) {
          std::string what_arg{"entire directive must appear on "
                               "one line"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } else {
          if (cur_char == character_constants::g_k_string_delimiter) {
            if (last_char_was_escape_leader == true) {
              last_char_was_escape_leader = false;
              file_path_str.push_back(cur_char);
            } else {
              last_state = args_location::closing_delimiter;
            }
          } else if (cur_char == character_constants::g_k_escape_leader) {
            last_char_was_escape_leader = true;
            file_path_str.push_back(cur_char);
          } else {
            file_path_str.push_back(cur_char);
          }
        }
      }
    } break;

    case args_location::closing_delimiter: {
      if (eof == true) {
        last_state = args_location::done;
      } else {
        if (start_pos_count.first != ctx.line_count) {
          last_state = args_location::done;
          ctx.file_stream.unget();
        } else {
          if (is_whitespace(cur_char,
                            character_constants::g_k_whitespace_chars) ==
              true) {
            last_state = args_location::trailing_whitespace;
          } else {
            std::string what_arg{"excess arguments given to "
                                 "include directive"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
          }
        }
      }
    } break;

    case args_location::trailing_whitespace: {
      if (eof == true) {
        last_state = args_location::done;
      } else {
        if (start_pos_count.first != ctx.line_count) {
          last_state = args_location::done;
          ctx.file_stream.unget();
        } else {
          if (is_whitespace(cur_char,
                            character_constants::g_k_whitespace_chars) ==
              true) {
            ;
          } else {
            std::string what_arg{"excess arguments given to "
                                 "include directive"};
            throw syntax_error::generate_formatted_error(
                what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
          }
        }
      }
    } break;

    case args_location::done: {
      throw std::runtime_error{"impossible!"};
    } break;
    }
  }

  if (file_path_str.empty() == true) {
    std::string what_arg{"empty file path argument given to "
                         "include directive"};
    throw syntax_error::generate_formatted_error(
        what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
  } else {
    std::variant<std::string, std::string::size_type> file_path_escaped{
        replace_escape_sequences(file_path_str)};

    switch (file_path_escaped.index()) {
    case 0: {
      std::filesystem::path file_path{
          std::get<std::string>(std::move(file_path_escaped))};
      if (file_path.is_absolute()) {
        return parser::parse(file_path);
      } else {
        return parser::parse(
            std::filesystem::path{ctx.file_path.parent_path() / file_path});
      }
    } break;

    case 1: {
      std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
          invalid_escape_sequence_pos_count{
              start_of_file_path_str_pos_count.first,
              static_cast<decltype(ctx.char_count)>(
                  start_of_file_path_str_pos_count.second +
                  std::get<std::string::size_type>(file_path_escaped))};

      std::string what_arg{"invalid escape sequence in include "
                           "directive argument"};
      throw syntax_error::generate_formatted_error(
          what_arg, ctx.file_path, invalid_escape_sequence_pos_count.first,
          invalid_escape_sequence_pos_count.second);
    } break;

    default: {
      throw std::runtime_error{"impossible!"};
    } break;
    }
  }
}

bool libconfigfile::parser::impl::handle_comments(context &ctx) {
  static_assert(character_constants::g_k_comment_cpp.front() ==
                character_constants::g_k_comment_c_start.front());
  static_assert(character_constants::g_k_comment_cpp.size() == 2);
  static_assert(character_constants::g_k_comment_c_start.size() == 2);
  static_assert(character_constants::g_k_comment_c_end.size() == 2);
  static constexpr char c_or_cpp_comment_leader{
      character_constants::g_k_comment_cpp.front()};

  char cur_char{};
  char peek_char{static_cast<char>(ctx.file_stream.peek())};

  switch (peek_char) {

  case character_constants::g_k_comment_script: {
    ctx.file_stream.get(cur_char);
    ++ctx.char_count;

    while (true) {
      ctx.file_stream.get(cur_char);
      ++ctx.char_count;
      if (ctx.file_stream.eof() == true) {
        return true;
      } else if (cur_char == character_constants::g_k_newline) {
        ctx.file_stream.unget();
        return true;
      }
    }
  } break;

  case c_or_cpp_comment_leader: {
    ctx.file_stream.get(cur_char);
    ++ctx.char_count;
    peek_char = ctx.file_stream.peek();

    switch (peek_char) {
    case character_constants::g_k_comment_cpp.back(): {
      ctx.file_stream.get(cur_char);
      ++ctx.char_count;

      while (true) {
        ctx.file_stream.get(cur_char);
        ++ctx.char_count;
        if (ctx.file_stream.eof() == true) {
          return true;
        } else if (cur_char == character_constants::g_k_newline) {
          ctx.file_stream.unget();
          return true;
        }
      }

    } break;

    case character_constants::g_k_comment_c_start.back(): {
      ctx.file_stream.get(cur_char);
      ++ctx.char_count;
      while (true) {
        ctx.file_stream.get(cur_char);
        ++ctx.char_count;
        if (ctx.file_stream.eof() == true) {
          std::string what_arg{"unterminated C-style comment"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } else if (cur_char == character_constants::g_k_newline) {
          ++ctx.line_count;
          ctx.char_count = 0;
        } else if (cur_char == character_constants::g_k_comment_c_end.front()) {
          if (static_cast<char>(ctx.file_stream.peek()) ==
              character_constants::g_k_comment_c_end.back()) {
            ctx.file_stream.get();
            ++ctx.char_count;
            return true;
          }
        }
      }
    } break;

    default: {
      ctx.file_stream.unget();
      --ctx.char_count;
      return false;
    } break;
    }
  } break;

  default: {
    return false;
  } break;
  }
}

char libconfigfile::parser::impl::handle_escape_sequence(context &ctx) {
  char escape_leader_char{};
  ctx.file_stream.get(escape_leader_char);
  if (ctx.file_stream.eof() == false) {
    ++ctx.char_count;
  }

  if (escape_leader_char == character_constants::g_k_escape_leader) {
    char escape_char_1{};
    ctx.file_stream.get(escape_char_1);

    if (ctx.file_stream.eof() == true) {
      std::string what_arg{"incomplete escape sequence"};
      throw syntax_error::generate_formatted_error(
          what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
    } else if (escape_char_1 == character_constants::g_k_newline) {
      std::string what_arg{"incomplete escape sequence"};
      throw syntax_error::generate_formatted_error(
          what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
    } else {
      ++ctx.char_count;

      if (escape_char_1 == character_constants::g_k_hex_escape_char) {
        char hex_digit_1{};
        ctx.file_stream.get(hex_digit_1);
        if (ctx.file_stream.eof() == true) {
          std::string what_arg{"incomplete escape sequence"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } else if (hex_digit_1 == character_constants::g_k_newline) {
          std::string what_arg{"incomplete escape sequence"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } else {
          ++ctx.char_count;
        }

        char hex_digit_2{};
        ctx.file_stream.get(hex_digit_2);
        if (ctx.file_stream.eof() == true) {
          std::string what_arg{"incomplete escape sequence"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } else if (hex_digit_2 == character_constants::g_k_newline) {
          std::string what_arg{"incomplete escape sequence"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        } else {
          ++ctx.char_count;
        }

        if ((is_digit(hex_digit_1, numeral_system_hexadecimal)) &&
            (is_digit(hex_digit_2, numeral_system_hexadecimal))) {
          std::string hex_string{std::string{} + hex_digit_1 + hex_digit_2};
          return static_cast<char>(
              std::stoi(hex_string, nullptr, numeral_system_hexadecimal.base));
        } else {
          std::string what_arg{"invalid digit in hexadecimal escape sequence"};
          throw syntax_error::generate_formatted_error(
              what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
        }
      } else {
        if (character_constants::g_k_basic_escape_chars.contains(
                escape_char_1)) {
          return character_constants::g_k_basic_escape_chars.at(escape_char_1);
        }
      }
    }
  } else {
    std::string what_arg{"expected escape sequence leader"};
    throw syntax_error::generate_formatted_error(
        what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
  }
}

std::variant<libconfigfile::value_node_type, libconfigfile::end_value_node_type>
libconfigfile::parser::impl::identify_key_value_value_type(
    context &ctx, const std::string &possible_terminating_chars,
    char *actual_terminating_char /*= nullptr*/) {

  std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
      pos_count_at_start{ctx.line_count, ctx.char_count};

  std::string gotten_chars{};
  char cur_char{};
  bool eof{false};

  const auto reset_context{[&ctx, &gotten_chars, &pos_count_at_start]() {
    for (auto i{gotten_chars.rbegin()}; i != gotten_chars.rend(); ++i) {
      ctx.file_stream.putback(*i);
    }
    ctx.line_count = pos_count_at_start.first;
    ctx.char_count = pos_count_at_start.second;
  }};

  while (true) {
    while (true) {
      handle_comments(ctx);
      ctx.file_stream.get(cur_char);
      if (ctx.file_stream.eof() == true) {
        eof = true;
        break;
      } else if (cur_char == character_constants::g_k_newline) {
        ++ctx.line_count;
        ctx.char_count = 0;
        continue;
      } else {
        ++ctx.char_count;
        break;
      }
    }

    if (eof == true) {
      std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
          pos_count_at_error{ctx.line_count, ctx.char_count};

      reset_context();

      std::string what_arg{"empty key-value value"};
      throw syntax_error::generate_formatted_error(
          what_arg, ctx.file_path, ctx.line_count, ctx.char_count);
    } else {
      gotten_chars.push_back(cur_char);

      if (is_whitespace(cur_char) == true) {
        continue;
      } else if (cur_char == character_constants::g_k_key_value_terminate) {
        std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
            pos_count_at_error{ctx.line_count, ctx.char_count};

        reset_context();

        std::string what_arg{"empty key-value value"};
        throw syntax_error::generate_formatted_error(what_arg, ctx.file_path,
                                                     pos_count_at_start.first,
                                                     pos_count_at_error.second);
      } else {
        reset_context();

        switch (cur_char) {
        case character_constants::g_k_array_opening_delimiter: {
          return value_node_type::ARRAY;
        } break;

        case character_constants::g_k_string_delimiter: {
          return end_value_node_type::STRING;
        } break;

        default: {
          return identify_key_value_numeric_value_type(
              ctx, possible_terminating_chars, actual_terminating_char);
        } break;
        }
      }
    }
  }
}

libconfigfile::end_value_node_type
libconfigfile::parser::impl::identify_key_value_numeric_value_type(
    context &ctx, const std::string &possible_terminating_chars,
    char *actual_terminating_char /*= nullptr*/) {

  std::string gotten_chars{};
  std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
      pos_count_at_start{ctx.line_count, ctx.char_count};
  char cur_char{};
  bool eof{false};
  while (true) {
    handle_comments(ctx);
    ctx.file_stream.get(cur_char);
    if (ctx.file_stream.eof() == true) {
      eof = true;
      break;
    } else {
      gotten_chars.push_back(cur_char);
      if (cur_char == character_constants::g_k_newline) {
        ++ctx.line_count;
        ctx.char_count = 0;
        continue;
      } else {
        ++ctx.char_count;
        if (possible_terminating_chars.find(cur_char) != std::string::npos) {
          if (actual_terminating_char != nullptr) {
            *actual_terminating_char = cur_char;
          };
          break;
        }
      }
    }
  }

  const auto reset_context{[&ctx, &gotten_chars, &pos_count_at_start]() {
    for (auto i{gotten_chars.rbegin()}; i != gotten_chars.rend(); ++i) {
      ctx.file_stream.putback(*i);
    }
    ctx.line_count = pos_count_at_start.first;
    ctx.char_count = pos_count_at_start.second;
  }};

  const auto cleanup_and_return{
      [&reset_context](const end_value_node_type ret_val) {
        reset_context();
        return ret_val;
      }};

  if ((case_insensitive_string_find(
           gotten_chars, character_constants::g_k_float_infinity.second) !=
       (std::string::npos)) ||
      (case_insensitive_string_find(
           gotten_chars, character_constants::g_k_float_not_a_number.second) !=
       (std::string::npos))) {
    return cleanup_and_return(end_value_node_type::FLOAT);
  } else {
    if ((gotten_chars.find(character_constants::g_k_float_decimal_point)) !=
        (std::string::npos)) {
      return cleanup_and_return(end_value_node_type::FLOAT);
    } else {
      if (((gotten_chars.find(
               character_constants::g_k_float_exponent_sign_lower)) !=
           (std::string::npos)) ||
          ((gotten_chars.find(
               character_constants::g_k_float_exponent_sign_upper)) !=
           (std::string::npos))) {
        if (((gotten_chars.find(numeral_system_hexadecimal.prefix)) !=
             (std::string::npos)) ||
            ((gotten_chars.find(numeral_system_hexadecimal.prefix_alt)) !=
             (std::string::npos))) {
          return cleanup_and_return(end_value_node_type::INTEGER);
        } else {
          return cleanup_and_return(end_value_node_type::FLOAT);
        }
      } else {
        return cleanup_and_return(end_value_node_type::INTEGER);
      }
    }
  }
}

std::variant<std::string /*result*/,
             std::string::size_type /*invalid_escape_sequence_pos_count*/>
libconfigfile::parser::impl::replace_escape_sequences(const std::string &str) {
  std::string result{};
  result.reserve(str.size());

  for (std::string::size_type cur_char{0}; cur_char < str.size(); ++cur_char) {
    if (str[cur_char] == character_constants::g_k_escape_leader) {
      std::string::size_type escape_char_pos_count{cur_char + 1};
      if (escape_char_pos_count < str.size()) {
        char escape_char{str[escape_char_pos_count]};
        if (escape_char == character_constants::g_k_hex_escape_char) {
          std::string::size_type hex_digit_pos_count_1{escape_char_pos_count +
                                                       1};
          std::string::size_type hex_digit_pos_count_2{escape_char_pos_count +
                                                       2};
          if ((hex_digit_pos_count_1 < str.size()) &&
              (hex_digit_pos_count_2 < str.size())) {
            char hex_digit_1{str[hex_digit_pos_count_1]};
            char hex_digit_2{str[hex_digit_pos_count_2]};
            if ((is_digit(hex_digit_1, numeral_system_hexadecimal)) &&
                (is_digit(hex_digit_2, numeral_system_hexadecimal))) {
              std::string hex_string{std::string{} + hex_digit_1 + hex_digit_2};
              result.push_back(
                  static_cast<char>(std::stoi(hex_string, nullptr, 16)));
              cur_char = hex_digit_pos_count_2;
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
             std::string::size_type /*unterminated_string_pos_count*/>
libconfigfile::parser::impl::extract_strings(
    const std::string &raw,
    const char delimiter /*=
               character_constants::g_k_string_delimiter*/
    ,
    const char delimiter_escape /*=
               character_constants::g_k_escape_leader*/
    ,
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
    const std::string &whitespace_chars /*=
                      character_constants::g_k_whitespace_chars*/
    ,
    bool trim_leading /*= true*/, bool trim_trailing /*= true*/) {
  if (str.empty() == true) {
    return "";
  } else {
    std::string::size_type start_pos_count{0};
    start_pos_count = str.find_first_not_of(whitespace_chars);

    std::string::size_type end_pos_count{str.size() - 1};
    end_pos_count = str.find_last_not_of(whitespace_chars);

    if ((trim_leading == false) && (trim_trailing == false)) {
      return str;
    } else if ((trim_leading == true) && (trim_trailing == false)) {
      return get_substr_between_indices_inclusive(str, start_pos_count,
                                                  std::string::npos);
    } else if ((trim_leading == false) && (trim_trailing == true)) {
      return get_substr_between_indices_inclusive(str, 0, end_pos_count);
    } else if ((trim_leading == true) && (trim_trailing == true)) {
      return get_substr_between_indices_inclusive(str, start_pos_count,
                                                  end_pos_count);
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

bool libconfigfile::parser::impl::case_insensitive_char_compare(char ch1,
                                                                char ch2) {
  return ((std::tolower(static_cast<unsigned char>(ch1))) ==
          (std::tolower(static_cast<unsigned char>(ch2))));
}

bool libconfigfile::parser::impl::case_insensitive_string_compare(
    const std::string &str1, const std::string &str2) {
  if (str1.size() == str2.size()) {
    for (std::string::size_type i{0}; i < str1.size(); ++i) {
      if (std::tolower(static_cast<unsigned char>(str1[i])) !=
          std::tolower(static_cast<unsigned char>(str1[i]))) {
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
        return ((std::tolower(static_cast<unsigned char>(ch1))) ==
                (std::tolower(static_cast<unsigned char>(ch2))));
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
    ret_val[i] =
        static_cast<char>(std::toupper(static_cast<unsigned char>(str[i])));
  }

  return ret_val;
}

std::string
libconfigfile::parser::impl::string_to_lower(const std::string &str) {
  std::string ret_val{};
  ret_val.resize(str.size());

  for (size_t i{0}; i < str.size(); ++i) {
    ret_val[i] =
        static_cast<char>(std::tolower(static_cast<unsigned char>(str[i])));
  }

  return ret_val;
}
