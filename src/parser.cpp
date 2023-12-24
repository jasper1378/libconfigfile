#include "parser.hpp"

#include "array_node.hpp"
#include "character_constants.hpp"
#include "constexpr_tolower_toupper.hpp"
#include "error_messages.hpp"
#include "float_node.hpp"
#include "integer_node.hpp"
#include "map_node.hpp"
#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"
#include "numeral_system.hpp"
#include "string_node.hpp"
#include "syntax_error.hpp"
#include "version.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <charconv>
#include <concepts>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <fstream>
#include <istream>
#include <optional>
#include <stdexcept>
#include <string>
#include <system_error>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

libconfigfile::node_ptr<libconfigfile::map_node>
libconfigfile::parser::parse(const std::string &identifier,
                             std::istream &input_stream,
                             const bool identifier_is_file_path /*= false*/) {
  return impl::parse(identifier, input_stream, identifier_is_file_path);
}

libconfigfile::node_ptr<libconfigfile::map_node>
libconfigfile::parser::parse_file(const char *file_path) {
  std::ifstream input_stream{file_path};
  return impl::parse(file_path, input_stream, true);
}

libconfigfile::node_ptr<libconfigfile::map_node>
libconfigfile::parser::parse_file(const std::string &file_path) {
  std::ifstream input_stream{file_path};
  return impl::parse(file_path, input_stream, true);
}

libconfigfile::node_ptr<libconfigfile::map_node>
libconfigfile::parser::parse_file(const std::filesystem::path &file_path) {
  std::ifstream input_stream{file_path};
  return impl::parse(file_path.string(), input_stream, true);
}

libconfigfile::node_ptr<libconfigfile::map_node>
libconfigfile::parser::impl::parse(
    const std::string &identifier, std::istream &input_stream,
    const bool identifier_is_file_path /*= false*/) {
  context ctx{identifier, input_stream, identifier_is_file_path, 1, 0};

  if (ctx.input_stream.good() == false) {
    throw std::runtime_error{
        std::string{} +
        ((identifier_is_file_path) ? ("file") : ("input stream")) + " \"" +
        ctx.identifier +
        "\" could not be opened for "
        "reading"};
  } else {
    node_ptr<map_node> ret_val{parse_map_value(ctx, "", nullptr, true)};
    ret_val->set_is_root_map(true);
    return ret_val;
  }
}

std::pair<std::string, libconfigfile::node_ptr<libconfigfile::node>>
libconfigfile::parser::impl::parse_key_value(
    context &ctx, const std::string &possible_terminating_chars,
    char *actual_terminating_char /*= nullptr*/) {
  std::pair<std::string, node_ptr<node>> ret_val{};

  ret_val.first = parse_key_value_key(ctx);
  ret_val.second = parse_key_value_value(ctx, possible_terminating_chars,
                                         actual_terminating_char);

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
      ctx.input_stream.get(cur_char);
      if (ctx.input_stream.eof() == true) {
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
        throw syntax_error{error_messages::err_msg_1_2_4.message,
                           error_messages::err_msg_1_2_4.category,
                           ctx.identifier, ctx.line_count, ctx.char_count};
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
              throw syntax_error{error_messages::err_msg_1_2_4.message,
                                 error_messages::err_msg_1_2_4.category,
                                 ctx.identifier, ctx.line_count,
                                 ctx.char_count};
            } break;

            case character_constants::g_k_key_value_terminate: {
              throw syntax_error{error_messages::err_msg_1_2_4.message,
                                 error_messages::err_msg_1_2_4.category,
                                 ctx.identifier, ctx.line_count,
                                 ctx.char_count};
            } break;

            default: {
              throw syntax_error{error_messages::err_msg_1_2_1.message,
                                 error_messages::err_msg_1_2_1.category,
                                 ctx.identifier, ctx.line_count,
                                 ctx.char_count};
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
        throw syntax_error{error_messages::err_msg_1_2_5.message,
                           error_messages::err_msg_1_2_5.category,
                           ctx.identifier, ctx.line_count, ctx.char_count};
      } else {
        if (is_whitespace(cur_char) == true) {
          last_state = key_name_location::trailing_whitespace;
        } else {
          if (cur_char == character_constants::g_k_key_value_assign) {
            last_state = key_name_location::equal_sign;
          } else if (cur_char == character_constants::g_k_key_value_terminate) {
            throw syntax_error{error_messages::err_msg_1_2_5.message,
                               error_messages::err_msg_1_2_5.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
          } else {
            if (start_of_name_proper_pos_count.first != ctx.line_count) {
              throw syntax_error{error_messages::err_msg_1_2_3.message,
                                 error_messages::err_msg_1_2_3.category,
                                 ctx.identifier, ctx.line_count,
                                 ctx.char_count};
            } else if (handled_comment_in_name_proper == true) {
              throw syntax_error{
                  error_messages::err_msg_1_2_2.message,
                  error_messages::err_msg_1_2_2.category, ctx.identifier,
                  pos_count_before_handled_comment_in_name_proper.first,
                  pos_count_before_handled_comment_in_name_proper.second};
            } else {
              if (is_invalid_character_valid_provided(
                      cur_char, character_constants::g_k_valid_name_chars) ==
                  true) {
                throw syntax_error{error_messages::err_msg_1_2_1.message,
                                   error_messages::err_msg_1_2_1.category,
                                   ctx.identifier, ctx.line_count,
                                   ctx.char_count};
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
        throw syntax_error{error_messages::err_msg_1_2_5.message,
                           error_messages::err_msg_1_2_5.category,
                           ctx.identifier, ctx.line_count, ctx.char_count};
      } else {
        if (is_whitespace(cur_char) == true) {
          ;
        } else {
          if (cur_char == character_constants::g_k_key_value_assign) {
            last_state = key_name_location::equal_sign;
          } else {
            if (cur_char == character_constants::g_k_key_value_terminate) {
              throw syntax_error{error_messages::err_msg_1_2_5.message,
                                 error_messages::err_msg_1_2_5.category,
                                 ctx.identifier, ctx.line_count,
                                 ctx.char_count};
            } else {
              throw syntax_error{error_messages::err_msg_1_2_5.message,
                                 error_messages::err_msg_1_2_5.category,
                                 ctx.identifier, ctx.line_count,
                                 ctx.char_count};
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

libconfigfile::node_ptr<libconfigfile::node>
libconfigfile::parser::impl::parse_key_value_value(
    context &ctx, const std::string &possible_terminating_chars,
    char *actual_terminating_char /*= nullptr*/) {
  bool first_loop{true};
  char cur_char{};

  for (; true; first_loop = false) {

    bool eof{false};
    while (true) {
      handle_comments(ctx);
      ctx.input_stream.get(cur_char);
      if (ctx.input_stream.eof() == true) {
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
      throw syntax_error{error_messages::err_msg_1_2_5.message,
                         error_messages::err_msg_1_2_5.category, ctx.identifier,
                         ctx.line_count, ctx.char_count};
    } else if (is_whitespace(cur_char) == true) {
      continue;
    } else if ((cur_char == character_constants::g_k_key_value_assign) &&
               (first_loop == true)) {
      continue;
      // } else if (cur_char == character_constants::g_k_key_value_terminate) {
    } else if (possible_terminating_chars.find(cur_char) != std::string::npos) {
      if (actual_terminating_char != nullptr) {
        *actual_terminating_char = cur_char;
      };
      throw syntax_error{error_messages::err_msg_1_2_5.message,
                         error_messages::err_msg_1_2_5.category, ctx.identifier,
                         ctx.line_count, ctx.char_count};
    } else {
      ctx.input_stream.unget();
      --ctx.char_count;
      return call_appropriate_value_parse_func(ctx, possible_terminating_chars,
                                               actual_terminating_char);
    }
  }
}

libconfigfile::node_ptr<libconfigfile::string_node>
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
      ctx.input_stream.get(cur_char);
      if (ctx.input_stream.eof() == true) {
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
      if (in_string == true) {
        throw syntax_error{error_messages::err_msg_1_3_3.message,
                           error_messages::err_msg_1_3_3.category,
                           ctx.identifier, ctx.line_count, ctx.char_count};
      } else {
        throw syntax_error{error_messages::err_msg_1_2_6.message,
                           error_messages::err_msg_1_2_6.category,
                           ctx.identifier, ctx.line_count, ctx.char_count};
      }
    } else {
      if (in_string == true) {
        if (last_opening_delimiter_pos_count.first != ctx.line_count) {
          throw syntax_error{error_messages::err_msg_1_3_2.message,
                             error_messages::err_msg_1_3_2.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        } else {
          if (cur_char == character_constants::g_k_string_delimiter) {
            in_string = false;
          } else if (cur_char == character_constants::g_k_escape_leader) {
            ctx.input_stream.unget();
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
          throw syntax_error{error_messages::err_msg_1_3_1.message,
                             error_messages::err_msg_1_3_1.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        }
      }
    }
  }

  if (in_string == true) {
    throw syntax_error{error_messages::err_msg_1_3_3.message,
                       error_messages::err_msg_1_3_3.category, ctx.identifier,
                       ctx.line_count, ctx.char_count};
  } else {
    return make_node_ptr<string_node>(std::move(string_contents));
  }
}

libconfigfile::node_ptr<libconfigfile::integer_node>
libconfigfile::parser::impl::parse_integer_value(
    context &ctx, const std::string &possible_terminating_chars,
    char *actual_terminating_char /*= nullptr*/) {
  static_assert(character_constants::g_k_num_sys_prefix_leader == '0');

  std::string actual_digits{};
  bool is_negative{false};
  const numeral_system *num_sys{nullptr};

  const std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
      pos_count_at_start{ctx.line_count, ctx.char_count};

  bool last_char_was_digit{false};
  bool any_digits_so_far{false};
  bool last_char_was_leading_zero{false};
  size_t num_of_leading_zeroes{0};
  bool in_trailing_whitespace{false};

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
          throw syntax_error{error_messages::err_msg_1_4_1.message,
                             error_messages::err_msg_1_4_1.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
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
      ctx.input_stream.get(cur_char);
      if (ctx.input_stream.eof() == true) {
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
      throw syntax_error{error_messages::err_msg_1_2_6.message,
                         error_messages::err_msg_1_2_6.category, ctx.identifier,
                         ctx.line_count, ctx.char_count};
    } else if (possible_terminating_chars.find(cur_char) != std::string::npos) {
      if (actual_terminating_char != nullptr) {
        *actual_terminating_char = cur_char;
      };
      break;
    } else if (pos_count_at_start.first != ctx.line_count) {
      throw syntax_error{error_messages::err_msg_1_4_4.message,
                         error_messages::err_msg_1_4_4.category, ctx.identifier,
                         ctx.line_count, ctx.char_count};
    } else if (handled_comment == true) {
      throw syntax_error{error_messages::err_msg_1_4_3.message,
                         error_messages::err_msg_1_4_3.category, ctx.identifier,
                         pos_count_before_handled_comment.first,
                         pos_count_before_handled_comment.second};
    } else if ((is_whitespace(cur_char) == true) &&
               (in_trailing_whitespace == false)) {
      in_trailing_whitespace = true;
    } else if ((is_whitespace(cur_char) == false) &&
               (in_trailing_whitespace == true)) {
      throw syntax_error{error_messages::err_msg_1_4_9.message,
                         error_messages::err_msg_1_4_9.category, ctx.identifier,
                         ctx.line_count, ctx.char_count};
    } else {
      switch (cur_char) {
      case character_constants::g_k_num_digit_separator: {
        if ((last_char_was_digit == false) ||
            (ctx.input_stream.peek() == std::istream::traits_type::eof())) {
          throw syntax_error{error_messages::err_msg_1_4_2.message,
                             error_messages::err_msg_1_4_2.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
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
          throw syntax_error{error_messages::err_msg_1_4_8.message,
                             error_messages::err_msg_1_4_8.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        }
      } break;

      case character_constants::g_k_num_negative_sign: {
        if (first_loop == true) {
          is_negative = true;
          last_char_was_digit = false;
          last_char_was_leading_zero = false;
        } else {
          throw syntax_error{error_messages::err_msg_1_4_6.message,
                             error_messages::err_msg_1_4_6.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
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
                throw syntax_error{error_messages::err_msg_1_4_7.message,
                                   error_messages::err_msg_1_4_7.category,
                                   ctx.identifier, ctx.line_count,
                                   ctx.char_count};
              }
            } else {
              throw syntax_error{error_messages::err_msg_1_4_1.message,
                                 error_messages::err_msg_1_4_1.category,
                                 ctx.identifier, ctx.line_count,
                                 ctx.char_count};
            }
          } else {
            throw syntax_error{error_messages::err_msg_1_4_7.message,
                               error_messages::err_msg_1_4_7.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
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
                throw syntax_error{error_messages::err_msg_1_4_7.message,
                                   error_messages::err_msg_1_4_7.category,
                                   ctx.identifier, ctx.line_count,
                                   ctx.char_count};
              }
            } else {
              throw syntax_error{error_messages::err_msg_1_4_1.message,
                                 error_messages::err_msg_1_4_1.category,
                                 ctx.identifier, ctx.line_count,
                                 ctx.char_count};
            }
          } else {
            throw syntax_error{error_messages::err_msg_1_4_7.message,
                               error_messages::err_msg_1_4_7.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
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
                throw syntax_error{error_messages::err_msg_1_4_7.message,
                                   error_messages::err_msg_1_4_7.category,
                                   ctx.identifier, ctx.line_count,
                                   ctx.char_count};
              }
            } else {
              throw syntax_error{error_messages::err_msg_1_4_1.message,
                                 error_messages::err_msg_1_4_1.category,
                                 ctx.identifier, ctx.line_count,
                                 ctx.char_count};
            }
          } else {
            throw syntax_error{error_messages::err_msg_1_4_7.message,
                               error_messages::err_msg_1_4_7.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
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

  node_ptr<integer_node> ret_val{nullptr};

  integer_node::base_t ret_val_value_buf{};
  if (std::from_chars(actual_digits.data(),
                      actual_digits.data() + actual_digits.size(),
                      ret_val_value_buf)
          .ec == std::errc::result_out_of_range) {
    throw syntax_error{error_messages::err_msg_1_4_5.message,
                       error_messages::err_msg_1_4_5.category, ctx.identifier,
                       pos_count_at_start.first, pos_count_at_start.second};
  } else {
    ret_val = make_node_ptr<integer_node>(ret_val_value_buf, num_sys);
  }

  if (is_negative == true) {
    ret_val->set(-(ret_val->get()));
  }

  return ret_val;
}

libconfigfile::node_ptr<libconfigfile::float_node>
libconfigfile::parser::impl::parse_float_value(
    context &ctx, const std::string &possible_terminating_chars,
    char *actual_terminating_char /*= nullptr*/) {
  std::string sanitized_string{};

  const std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
      pos_count_at_start{ctx.line_count, ctx.char_count};

  static const std::unordered_map<std::string, float_node::base_t>
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

  bool in_trailing_whitespace{false};

  for (;;) {
    char cur_char{};
    bool eof{false};
    bool handled_comment{};
    std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
        pos_count_before_handled_comment{};
    while (true) {
      pos_count_before_handled_comment = {ctx.line_count, ctx.char_count};
      handled_comment = handle_comments(ctx);
      ctx.input_stream.get(cur_char);
      if (ctx.input_stream.eof() == true) {
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
      throw syntax_error{error_messages::err_msg_1_2_6.message,
                         error_messages::err_msg_1_2_6.category, ctx.identifier,
                         ctx.line_count, ctx.char_count};
    } else if (possible_terminating_chars.find(cur_char) != std::string::npos) {
      if (actual_terminating_char != nullptr) {
        *actual_terminating_char = cur_char;
      };
      break;
    } else if (pos_count_at_start.first != ctx.line_count) {
      throw syntax_error{error_messages::err_msg_1_5_9.message,
                         error_messages::err_msg_1_5_9.category, ctx.identifier,
                         ctx.line_count, ctx.char_count};
    } else if (handled_comment == true) {
      throw syntax_error{error_messages::err_msg_1_5_8.message,
                         error_messages::err_msg_1_5_8.category, ctx.identifier,
                         pos_count_before_handled_comment.first,
                         pos_count_before_handled_comment.second};
    } else if ((is_whitespace(cur_char) == true) &&
               (in_trailing_whitespace == false)) {
      in_trailing_whitespace = true;
    } else if ((is_whitespace(cur_char) == false) &&
               (in_trailing_whitespace == true)) {
      throw syntax_error{error_messages::err_msg_1_5_13.message,
                         error_messages::err_msg_1_5_13.category,
                         ctx.identifier, ctx.line_count, ctx.char_count};
    } else {

      switch (cur_location) {

      case num_location::integer: {
        switch (cur_char) {

        case libconfigfile::tolower<
            character_constants::g_k_float_infinity.second.front()>():
        case libconfigfile::toupper<
            character_constants::g_k_float_infinity.second.front()>(): {
          const std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
              pos_count_at_start_of_special_float{ctx.line_count,
                                                  ctx.char_count};

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
                ctx.input_stream.get(cur_char);
                if (ctx.input_stream.eof() == true) {
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
                throw syntax_error{error_messages::err_msg_1_2_6.message,
                                   error_messages::err_msg_1_2_6.category,
                                   ctx.identifier, ctx.line_count,
                                   ctx.char_count};
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
              node_ptr<float_node> ret_val{nullptr};

              if (last_char == char_type::negative) {
                ret_val = make_node_ptr<float_node>(
                    -(character_constants::g_k_float_infinity.first));
              } else {
                ret_val = make_node_ptr<float_node>(
                    character_constants::g_k_float_infinity.first);
              }

              return ret_val;
            } else {
              throw syntax_error{error_messages::err_msg_1_5_1.message,
                                 error_messages::err_msg_1_5_1.category,
                                 ctx.identifier,
                                 pos_count_at_start_of_special_float.first,
                                 pos_count_at_start_of_special_float.second};
            }
          } else {
            throw syntax_error{error_messages::err_msg_1_5_1.message,
                               error_messages::err_msg_1_5_1.category,
                               ctx.identifier,
                               pos_count_at_start_of_special_float.first,
                               pos_count_at_start_of_special_float.second};
          }
        } break;

        case libconfigfile::tolower<
            character_constants::g_k_float_not_a_number.second.front()>():
        case libconfigfile::toupper<
            character_constants::g_k_float_not_a_number.second.front()>(): {
          const std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
              pos_count_at_start_of_special_float{ctx.line_count,
                                                  ctx.char_count};

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
                ctx.input_stream.get(cur_char);
                if (ctx.input_stream.eof() == true) {
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
                throw syntax_error{error_messages::err_msg_1_2_6.message,
                                   error_messages::err_msg_1_2_6.category,
                                   ctx.identifier, ctx.line_count,
                                   ctx.char_count};
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
              node_ptr<float_node> ret_val{nullptr};

              if (last_char == char_type::negative) {
                ret_val = make_node_ptr<float_node>(
                    -(character_constants::g_k_float_not_a_number.first));
              } else {
                ret_val = make_node_ptr<float_node>(
                    character_constants::g_k_float_not_a_number.first);
              }

              return ret_val;
            } else {
              throw syntax_error{error_messages::err_msg_1_5_1.message,
                                 error_messages::err_msg_1_5_1.category,
                                 ctx.identifier,
                                 pos_count_at_start_of_special_float.first,
                                 pos_count_at_start_of_special_float.second};
            }
          } else {
            throw syntax_error{error_messages::err_msg_1_5_1.message,
                               error_messages::err_msg_1_5_1.category,
                               ctx.identifier,
                               pos_count_at_start_of_special_float.first,
                               pos_count_at_start_of_special_float.second};
          }
        } break;

        case character_constants::g_k_num_positive_sign: {
          if (last_char == char_type::start) {
            last_char = char_type::positive;
            sanitized_string.push_back(cur_char);
          } else {
            throw syntax_error{error_messages::err_msg_1_5_12.message,
                               error_messages::err_msg_1_5_12.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
          }
        } break;

        case character_constants::g_k_num_negative_sign: {
          if (last_char == char_type::start) {
            last_char = char_type::negative;
            sanitized_string.push_back(cur_char);
          } else {
            throw syntax_error{error_messages::err_msg_1_5_11.message,
                               error_messages::err_msg_1_5_11.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
          }
        } break;

        case character_constants::g_k_num_digit_separator: {
          if (last_char == char_type::digit) {
            if (is_digit(static_cast<char>(ctx.input_stream.peek()),
                         numeral_system_decimal) == true) {
              last_char = char_type::separator;
            } else {
              throw syntax_error{error_messages::err_msg_1_5_6.message,
                                 error_messages::err_msg_1_5_6.category,
                                 ctx.identifier, ctx.line_count,
                                 ctx.char_count};
            }
          } else {
            throw syntax_error{error_messages::err_msg_1_5_6.message,
                               error_messages::err_msg_1_5_6.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
          }
        } break;

        case character_constants::g_k_float_decimal_point: {
          if (last_char == char_type::digit) {
            if (is_digit(static_cast<char>(ctx.input_stream.peek()),
                         numeral_system_decimal) == true) {
              last_char = char_type::decimal;
              cur_location = num_location::fractional;
              sanitized_string.push_back(cur_char);
            } else {
              throw syntax_error{error_messages::err_msg_1_5_5.message,
                                 error_messages::err_msg_1_5_5.category,
                                 ctx.identifier, ctx.line_count,
                                 ctx.char_count};
            }
          } else {
            throw syntax_error{error_messages::err_msg_1_5_5.message,
                               error_messages::err_msg_1_5_5.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
          }
        } break;

        case character_constants::g_k_float_exponent_sign_lower:
        case character_constants::g_k_float_exponent_sign_upper: {
          if (last_char == char_type::digit) {
            const char next_char{static_cast<char>(ctx.input_stream.peek())};
            if ((is_digit(next_char, numeral_system_decimal) == true) ||
                (next_char == character_constants::g_k_num_positive_sign) ||
                (next_char == character_constants::g_k_num_negative_sign)) {
              last_char = char_type::exponent;
              cur_location = num_location::exponent;
              sanitized_string.push_back(cur_char);
            } else {
              throw syntax_error{error_messages::err_msg_1_5_7.message,
                                 error_messages::err_msg_1_5_7.category,
                                 ctx.identifier, ctx.line_count,
                                 ctx.char_count};
            }
          } else {
            throw syntax_error{error_messages::err_msg_1_5_7.message,
                               error_messages::err_msg_1_5_7.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
          }
        } break;

        default: {
          if (is_digit(cur_char, numeral_system_decimal) == true) {
            last_char = char_type::digit;
            sanitized_string.push_back(cur_char);
          } else {
            throw syntax_error{error_messages::err_msg_1_5_1.message,
                               error_messages::err_msg_1_5_1.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
          }
        } break;
        }
      } break;

      case num_location::fractional: {
        switch (cur_char) {

        case character_constants::g_k_num_positive_sign: {
          throw syntax_error{error_messages::err_msg_1_5_12.message,
                             error_messages::err_msg_1_5_12.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        } break;

        case character_constants::g_k_num_negative_sign: {
          throw syntax_error{error_messages::err_msg_1_5_11.message,
                             error_messages::err_msg_1_5_11.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        } break;

        case character_constants::g_k_num_digit_separator: {
          if (last_char == char_type::digit) {
            if (is_digit(static_cast<char>(ctx.input_stream.peek()),
                         numeral_system_decimal) == true) {
              last_char = char_type::separator;
            } else {
              throw syntax_error{error_messages::err_msg_1_5_6.message,
                                 error_messages::err_msg_1_5_6.category,
                                 ctx.identifier, ctx.line_count,
                                 ctx.char_count};
            }
          } else {
            throw syntax_error{error_messages::err_msg_1_5_6.message,
                               error_messages::err_msg_1_5_6.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
          }
        } break;

        case character_constants::g_k_float_decimal_point: {
          throw syntax_error{error_messages::err_msg_1_5_2.message,
                             error_messages::err_msg_1_5_2.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        } break;

        case character_constants::g_k_float_exponent_sign_lower:
        case character_constants::g_k_float_exponent_sign_upper: {
          if (last_char == char_type::digit) {
            const char next_char{static_cast<char>(ctx.input_stream.peek())};
            if ((is_digit(next_char, numeral_system_decimal) == true) ||
                (next_char == character_constants::g_k_num_positive_sign) ||
                (next_char == character_constants::g_k_num_negative_sign)) {
              last_char = char_type::exponent;
              cur_location = num_location::exponent;
              sanitized_string.push_back(cur_char);
            } else {
              throw syntax_error{error_messages::err_msg_1_5_7.message,
                                 error_messages::err_msg_1_5_7.category,
                                 ctx.identifier, ctx.line_count,
                                 ctx.char_count};
            }
          } else {
            throw syntax_error{error_messages::err_msg_1_5_7.message,
                               error_messages::err_msg_1_5_7.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
          }
        } break;

        default: {
          if (is_digit(cur_char, numeral_system_decimal) == true) {
            last_char = char_type::digit;
            sanitized_string.push_back(cur_char);
          } else {
            throw syntax_error{error_messages::err_msg_1_5_1.message,
                               error_messages::err_msg_1_5_1.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
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
            throw syntax_error{error_messages::err_msg_1_5_12.message,
                               error_messages::err_msg_1_5_12.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
          }
        } break;

        case character_constants::g_k_num_negative_sign: {
          if (last_char == char_type::exponent) {
            last_char = char_type::positive;
            sanitized_string.push_back(cur_char);
          } else {
            throw syntax_error{error_messages::err_msg_1_5_11.message,
                               error_messages::err_msg_1_5_11.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
          }
        } break;

        case character_constants::g_k_num_digit_separator: {
          if (last_char == char_type::digit) {
            if (is_digit(static_cast<char>(ctx.input_stream.peek()),
                         numeral_system_decimal) == true) {
              last_char = char_type::separator;
            } else {
              throw syntax_error{error_messages::err_msg_1_5_6.message,
                                 error_messages::err_msg_1_5_6.category,
                                 ctx.identifier, ctx.line_count,
                                 ctx.char_count};
            }
          } else {
            throw syntax_error{error_messages::err_msg_1_5_6.message,
                               error_messages::err_msg_1_5_6.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
          }
        } break;

        case character_constants::g_k_float_decimal_point: {
          throw syntax_error{error_messages::err_msg_1_5_4.message,
                             error_messages::err_msg_1_5_4.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        } break;

        case character_constants::g_k_float_exponent_sign_lower:
        case character_constants::g_k_float_exponent_sign_upper: {
          throw syntax_error{error_messages::err_msg_1_5_3.message,
                             error_messages::err_msg_1_5_3.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        } break;

        default: {
          if (is_digit(cur_char, numeral_system_decimal) == true) {
            last_char = char_type::digit;
            sanitized_string.push_back(cur_char);
          } else {
            throw syntax_error{error_messages::err_msg_1_5_1.message,
                               error_messages::err_msg_1_5_1.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
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

  node_ptr<float_node> ret_val{nullptr};

  float_node::base_t ret_val_value_buf{};
  if (std::from_chars(((sanitized_string.front() ==
                        character_constants::g_k_num_positive_sign)
                           ? (sanitized_string.data() + 1)
                           : (sanitized_string.data())),
                      sanitized_string.data() + sanitized_string.size(),
                      ret_val_value_buf)
          .ec == std::errc::result_out_of_range) {
    throw syntax_error{error_messages::err_msg_1_5_10.message,
                       error_messages::err_msg_1_5_10.category, ctx.identifier,
                       ctx.line_count, ctx.char_count};

  } else {
    ret_val = make_node_ptr<float_node>(ret_val_value_buf);
  }

  return ret_val;
}

libconfigfile::node_ptr<libconfigfile::array_node>
libconfigfile::parser::impl::parse_array_value(
    context &ctx, const std::string &possible_terminating_chars,
    char *actual_terminating_char /*= nullptr*/) {
  node_ptr<libconfigfile::array_node> ret_val{make_node_ptr<array_node>()};

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
      ctx.input_stream.get(cur_char);
      if (ctx.input_stream.eof() == true) {
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
      throw syntax_error{error_messages::err_msg_1_2_6.message,
                         error_messages::err_msg_1_2_6.category, ctx.identifier,
                         ctx.line_count, ctx.char_count};
    } else if ((possible_terminating_chars.find(cur_char) !=
                std::string::npos) &&
               (last_char_type == char_type::closing_delimiter)) {
      if (actual_terminating_char != nullptr) {
        *actual_terminating_char = cur_char;
      }
      break;
    } else if (is_whitespace(cur_char)) {
      continue;
    } else {
      switch (last_char_type) {

      case char_type::leading_whitespace: {
        if (cur_char == character_constants::g_k_array_opening_delimiter) {
          last_char_type = char_type::opening_delimiter;
        } else {
          throw syntax_error{error_messages::err_msg_1_6_1.message,
                             error_messages::err_msg_1_6_1.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        }
      } break;

      case char_type::opening_delimiter: {
        if (cur_char == character_constants::g_k_array_closing_delimiter) {
          last_char_type = char_type::closing_delimiter;
        } else if (cur_char ==
                   character_constants::g_k_array_element_separator) {
          throw syntax_error{error_messages::err_msg_1_2_5.message,
                             error_messages::err_msg_1_2_5.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        } else {
          --ctx.char_count;
          ctx.input_stream.unget();
          char element_actual_terminating_char{};
          ret_val->push_back(call_appropriate_value_parse_func(
              ctx, possible_terminating_chars_for_elements,
              &element_actual_terminating_char));
          switch (element_actual_terminating_char) {
          case character_constants::g_k_array_element_separator: {
            last_char_type = char_type::element_separator;
          } break;
          case character_constants::g_k_array_closing_delimiter: {
            last_char_type = char_type::closing_delimiter;
          } break;
          default: {
            throw std::runtime_error{"impossible!"};
          } break;
          }
        }
      } break;

      case char_type::element_separator: {
        if (cur_char == character_constants::g_k_array_closing_delimiter) {
          last_char_type = char_type::closing_delimiter;
        } else if (cur_char ==
                   character_constants::g_k_array_element_separator) {
          throw syntax_error{error_messages::err_msg_1_2_5.message,
                             error_messages::err_msg_1_2_5.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        } else {
          --ctx.char_count;
          ctx.input_stream.unget();
          char element_actual_terminating_char{};
          ret_val->push_back(call_appropriate_value_parse_func(
              ctx, possible_terminating_chars_for_elements,
              &element_actual_terminating_char));
          switch (element_actual_terminating_char) {
          case character_constants::g_k_array_element_separator: {
            last_char_type = char_type::element_separator;
          } break;
          case character_constants::g_k_array_closing_delimiter: {
            last_char_type = char_type::closing_delimiter;
          } break;
          default: {
            throw std::runtime_error{"impossible!"};
          } break;
          }
        }
      } break;

      case char_type::closing_delimiter: {
        throw syntax_error{error_messages::err_msg_1_6_3.message,
                           error_messages::err_msg_1_6_3.category,
                           ctx.identifier, ctx.line_count, ctx.char_count};
      } break;
      }
    }
  }
  return ret_val;
}

libconfigfile::node_ptr<libconfigfile::map_node>
libconfigfile::parser::impl::parse_map_value(
    context &ctx, const std::string &possible_terminating_chars,
    char *actual_terminating_char /*= nullptr*/,
    const bool is_root_map /*= false*/) {
  node_ptr<map_node> ret_val{make_node_ptr<map_node>()};

  if (is_root_map == true) {
    const typename std::istream::int_type first_char{ctx.input_stream.peek()};
    if (first_char == std::istream::traits_type::eof()) {
      return ret_val;
    }
  }

  enum class char_type {
    leading_whitespace,
    opening_delimiter,
    member_separator,
    closing_delimiter,
  };

  char_type last_char_type{((is_root_map == true)
                                ? (char_type::opening_delimiter)
                                : (char_type::leading_whitespace))};

  decltype(ctx.line_count) last_non_whitespace_char_line_pos_count{};

  const auto handle_directive{[is_root_map,
                               &last_non_whitespace_char_line_pos_count, &ctx,
                               &ret_val]() {
    if (is_root_map == false) {
      throw syntax_error{error_messages::err_msg_1_8_15.message,
                         error_messages::err_msg_1_8_15.category,
                         ctx.identifier, ctx.line_count, ctx.char_count};
    } else if (last_non_whitespace_char_line_pos_count == ctx.line_count) {
      throw syntax_error{error_messages::err_msg_1_8_16.message,
                         error_messages::err_msg_1_8_16.category,
                         ctx.identifier, ctx.line_count, ctx.char_count};
    } else {

      const std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
          start_pos_count;
      ctx.input_stream.unget();
      --ctx.char_count;
      std::pair<directive, std::optional<node_ptr<map_node>>> dir_res{
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
          if (ret_val->contains(i->first)) {
            throw syntax_error{error_messages::err_msg_1_9_5.message,
                               error_messages::err_msg_1_9_5.category,
                               ctx.identifier, start_pos_count.first,
                               start_pos_count.second};
          }
        }

        ret_val->insert(
            std::make_move_iterator(dir_res.second.value()->begin()),
            std::make_move_iterator(dir_res.second.value()->end()));
      } break;
      }
    }
  }};

  for (;;) {
    char cur_char{};
    bool eof{false};
    while (true) {
      handle_comments(ctx);
      ctx.input_stream.get(cur_char);
      if (ctx.input_stream.eof() == true) {
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
      if (is_root_map == true) {
        break;
      } else {
        throw syntax_error{error_messages::err_msg_1_2_6.message,
                           error_messages::err_msg_1_2_6.category,
                           ctx.identifier, ctx.line_count, ctx.char_count};
      }
    } else if ((possible_terminating_chars.find(cur_char)) !=
               (std::string::npos)) {
      if (actual_terminating_char != nullptr) {
        *actual_terminating_char = cur_char;
      }
      break;
    } else if (is_whitespace(cur_char)) {
      continue;
    } else {
      if ((is_root_map == true) &&
          (cur_char != character_constants::g_k_directive_leader)) {
        last_non_whitespace_char_line_pos_count = ctx.line_count;
      }

      switch (last_char_type) {

      case char_type::leading_whitespace: {
        if (cur_char == character_constants::g_k_map_opening_delimiter) {
          last_char_type = char_type::opening_delimiter;
        } else {
          throw syntax_error{error_messages::err_msg_1_7_1.message,
                             error_messages::err_msg_1_7_1.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        }
      } break;

      case char_type::opening_delimiter: {
        if ((cur_char == character_constants::g_k_map_closing_delimiter) &&
            (is_root_map == false)) {
          last_char_type = char_type::closing_delimiter;
        } else if (cur_char == character_constants::g_k_key_value_terminate) {
          throw syntax_error{error_messages::err_msg_1_2_7.message,
                             error_messages::err_msg_1_2_7.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        } else if (cur_char == character_constants::g_k_directive_leader) {
          handle_directive();
        } else {
          const std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
              start_pos_count{ctx.line_count, ctx.char_count};
          ctx.input_stream.unget();
          --ctx.char_count;

          std::pair<std::string, node_ptr<node>> new_key_value{parse_key_value(
              ctx, std::string{character_constants::g_k_key_value_terminate})};

          if (ret_val->contains(new_key_value.first) == true) {
            throw syntax_error{error_messages::err_msg_1_9_5.message,
                               error_messages::err_msg_1_9_5.category,
                               ctx.identifier, start_pos_count.first,
                               start_pos_count.second};
          } else {
            ret_val->insert({std::move(new_key_value)});
          }
          last_char_type = char_type::member_separator;
        }
      } break;

      case char_type::member_separator: {
        if ((cur_char == character_constants::g_k_map_closing_delimiter) &&
            (is_root_map == false)) {
          last_char_type = char_type::closing_delimiter;
        } else if (cur_char == character_constants::g_k_key_value_terminate) {
          throw syntax_error{error_messages::err_msg_1_2_7.message,
                             error_messages::err_msg_1_2_7.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        } else if (cur_char == character_constants::g_k_directive_leader) {
          handle_directive();
        } else {
          const std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
              start_pos_count{ctx.line_count, ctx.char_count};
          ctx.input_stream.unget();
          --ctx.char_count;

          std::pair<std::string, node_ptr<node>> new_key_value{parse_key_value(
              ctx, std::string{character_constants::g_k_key_value_terminate})};

          if (ret_val->contains(new_key_value.first) == true) {
            throw syntax_error{error_messages::err_msg_1_9_5.message,
                               error_messages::err_msg_1_9_5.category,
                               ctx.identifier, start_pos_count.first,
                               start_pos_count.second};
          } else {
            ret_val->insert({std::move(new_key_value)});
          }
          last_char_type = char_type::member_separator;
        }
      } break;

      case char_type::closing_delimiter: {
        throw syntax_error{error_messages::err_msg_1_7_3.message,
                           error_messages::err_msg_1_7_3.category,
                           ctx.identifier, ctx.line_count, ctx.char_count};
      } break;
      }
    }
  }
  return ret_val;
}
libconfigfile::node_ptr<libconfigfile::node>
libconfigfile::parser::impl::call_appropriate_value_parse_func(
    context &ctx, const std::string &possible_terminating_chars,
    char *actual_terminating_char /*= nullptr*/) {

  node_type value_type{identify_key_value_value_type(
      ctx, possible_terminating_chars, actual_terminating_char)};

  switch (value_type) {
  case node_type::Map: {
    return node_ptr_cast<node>(parse_map_value(ctx, possible_terminating_chars,
                                               actual_terminating_char));
  } break;

  case node_type::Array: {
    return node_ptr_cast<node>(parse_array_value(
        ctx, possible_terminating_chars, actual_terminating_char));
  } break;

  case node_type::String: {
    return node_ptr_cast<node>(parse_string_value(
        ctx, possible_terminating_chars, actual_terminating_char));
  } break;

  case node_type::Integer: {
    return node_ptr_cast<node>(parse_integer_value(
        ctx, possible_terminating_chars, actual_terminating_char));
  } break;

  case node_type::Float: {
    return node_ptr_cast<node>(parse_float_value(
        ctx, possible_terminating_chars, actual_terminating_char));
  } break;

  default: {
    throw std::runtime_error{"impossible!"};
  } break;
  }
}

std::pair<libconfigfile::parser::impl::directive,
          std::optional<libconfigfile::node_ptr<libconfigfile::map_node>>>
libconfigfile::parser::impl::parse_directive(context &ctx) {
  const std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
      start_pos_count{ctx.line_count, ctx.char_count};

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
      ctx.input_stream.get(cur_char);
      if (ctx.input_stream.eof() == true) {
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
        throw syntax_error{error_messages::err_msg_1_8_3.message,
                           error_messages::err_msg_1_8_3.category,
                           ctx.identifier, ctx.line_count, ctx.char_count};
      } else {
        if (cur_char == character_constants::g_k_directive_leader) {
          ;
        } else if (is_whitespace(cur_char,
                                 character_constants::g_k_whitespace_chars) ==
                   true) {
          last_state = name_location::leading_whitespace;
          ;
        } else {
          if (ctx.line_count != start_pos_count.first) {
            throw syntax_error{error_messages::err_msg_1_8_2.message,
                               error_messages::err_msg_1_8_2.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
          } else {
            last_state = name_location::name_proper;
            name.push_back(cur_char);
          }
        }
      }
    } break;

    case name_location::leading_whitespace: {
      if (eof == true) {
        throw syntax_error{error_messages::err_msg_1_8_3.message,
                           error_messages::err_msg_1_8_3.category,
                           ctx.identifier, ctx.line_count, ctx.char_count};
      } else {
        if (is_whitespace(cur_char,
                          character_constants::g_k_whitespace_chars) == true) {
          ;
        } else {
          if (ctx.line_count != start_pos_count.first) {
            throw syntax_error{error_messages::err_msg_1_8_2.message,
                               error_messages::err_msg_1_8_2.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
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
          if (ctx.line_count != start_pos_count.first) {
            throw syntax_error{error_messages::err_msg_1_8_2.message,
                               error_messages::err_msg_1_8_2.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
          } else if (handled_comment_in_name_proper == true) {
            throw syntax_error{
                error_messages::err_msg_1_8_4.message,
                error_messages::err_msg_1_8_4.category, ctx.identifier,
                pos_count_before_handled_comment_in_name_proper.first,
                pos_count_before_handled_comment_in_name_proper.second};
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
    throw syntax_error{error_messages::err_msg_1_8_1.message,
                       error_messages::err_msg_1_8_1.category, ctx.identifier,
                       ctx.line_count, ctx.char_count};
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
  const std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
      start_pos_count{ctx.line_count, ctx.char_count};

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
      ctx.input_stream.get(cur_char);
      if (ctx.input_stream.eof() == true) {
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
        throw syntax_error{error_messages::err_msg_1_8_13.message,
                           error_messages::err_msg_1_8_13.category,
                           ctx.identifier, ctx.line_count, ctx.char_count};
      } else {
        if (is_whitespace(cur_char,
                          character_constants::g_k_whitespace_chars) == true) {
          ;
        } else if (cur_char == character_constants::g_k_string_delimiter) {
          if (start_pos_count.first != ctx.line_count) {
            throw syntax_error{error_messages::err_msg_1_8_2.message,
                               error_messages::err_msg_1_8_2.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
          } else {
            last_state = args_location::opening_delimiter;
          }
        } else {
          throw syntax_error{error_messages::err_msg_1_8_13.message,
                             error_messages::err_msg_1_8_13.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        }
      }
    } break;

    case args_location::opening_delimiter: {
      if (eof == true) {
        throw syntax_error{error_messages::err_msg_1_8_14.message,
                           error_messages::err_msg_1_8_14.category,
                           ctx.identifier, ctx.line_count, ctx.char_count};
      } else {
        if (start_pos_count.first != ctx.line_count) {
          throw syntax_error{error_messages::err_msg_1_8_2.message,
                             error_messages::err_msg_1_8_2.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
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
        throw syntax_error{error_messages::err_msg_1_8_14.message,
                           error_messages::err_msg_1_8_14.category,
                           ctx.identifier, ctx.line_count, ctx.char_count};
      } else {
        if (start_pos_count.first != ctx.line_count) {
          throw syntax_error{error_messages::err_msg_1_8_2.message,
                             error_messages::err_msg_1_8_2.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
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
          ctx.input_stream.unget();
          --ctx.char_count;
        } else {
          if (is_whitespace(cur_char,
                            character_constants::g_k_whitespace_chars) ==
              true) {
            last_state = args_location::trailing_whitespace;
          } else {
            throw syntax_error{error_messages::err_msg_1_8_11.message,
                               error_messages::err_msg_1_8_11.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
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
          ctx.input_stream.unget();
          --ctx.char_count;
        } else {
          if (is_whitespace(cur_char,
                            character_constants::g_k_whitespace_chars) ==
              true) {
            ;
          } else {
            throw syntax_error{error_messages::err_msg_1_8_11.message,
                               error_messages::err_msg_1_8_11.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
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
    throw syntax_error{error_messages::err_msg_1_8_12.message,
                       error_messages::err_msg_1_8_12.category, ctx.identifier,
                       ctx.line_count, ctx.char_count};
  } else {
    if (version_str == g_k_version) {
      return;
    } else {
      throw syntax_error{error_messages::err_msg_1_8_10.message,
                         error_messages::err_msg_1_8_10.category,
                         ctx.identifier, start_of_version_str_pos_count.first,
                         start_of_version_str_pos_count.second};
    }
  }
}

libconfigfile::node_ptr<libconfigfile::map_node>
libconfigfile::parser::impl::parse_include_directive(context &ctx) {
  const std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
      start_pos_count{ctx.line_count, ctx.char_count};

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
      ctx.input_stream.get(cur_char);
      if (ctx.input_stream.eof() == true) {
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
        throw syntax_error{error_messages::err_msg_1_8_7.message,
                           error_messages::err_msg_1_8_7.category,
                           ctx.identifier, ctx.line_count, ctx.char_count};
      } else {
        if (is_whitespace(cur_char,
                          character_constants::g_k_whitespace_chars) == true) {
          ;
        } else if (cur_char == character_constants::g_k_string_delimiter) {
          if (start_pos_count.first != ctx.line_count) {
            throw syntax_error{error_messages::err_msg_1_8_2.message,
                               error_messages::err_msg_1_8_2.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
          } else {
            last_state = args_location::opening_delimiter;
          }
        } else {
          throw syntax_error{error_messages::err_msg_1_8_7.message,
                             error_messages::err_msg_1_8_7.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        }
      }
    } break;

    case args_location::opening_delimiter: {
      if (eof == true) {
        throw syntax_error{error_messages::err_msg_1_8_8.message,
                           error_messages::err_msg_1_8_8.category,
                           ctx.identifier, ctx.line_count, ctx.char_count};
      } else {
        if (start_pos_count.first != ctx.line_count) {
          throw syntax_error{error_messages::err_msg_1_8_2.message,
                             error_messages::err_msg_1_8_2.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
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
        throw syntax_error{error_messages::err_msg_1_8_8.message,
                           error_messages::err_msg_1_8_8.category,
                           ctx.identifier, ctx.line_count, ctx.char_count};
      } else {
        if (start_pos_count.first != ctx.line_count) {
          throw syntax_error{error_messages::err_msg_1_8_2.message,
                             error_messages::err_msg_1_8_2.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        } else {
          if (cur_char == character_constants::g_k_string_delimiter) {
            if (last_char_was_escape_leader == true) {
              last_char_was_escape_leader = false;
              file_path_str.push_back(cur_char);
            } else {
              last_char_was_escape_leader = false;
              last_char_was_escape_leader = false;
              last_state = args_location::closing_delimiter;
            }
          } else if (cur_char == character_constants::g_k_escape_leader) {
            last_char_was_escape_leader = true;
            file_path_str.push_back(cur_char);
          } else {
            last_char_was_escape_leader = false;
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
          ctx.input_stream.unget();
          --ctx.char_count;
        } else {
          if (is_whitespace(cur_char,
                            character_constants::g_k_whitespace_chars) ==
              true) {
            last_state = args_location::trailing_whitespace;
          } else {
            throw syntax_error{error_messages::err_msg_1_8_9.message,
                               error_messages::err_msg_1_8_9.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
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
          ctx.input_stream.unget();
          --ctx.char_count;
        } else {
          if (is_whitespace(cur_char,
                            character_constants::g_k_whitespace_chars) ==
              true) {
            ;
          } else {
            throw syntax_error{error_messages::err_msg_1_8_9.message,
                               error_messages::err_msg_1_8_9.category,
                               ctx.identifier, ctx.line_count, ctx.char_count};
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
    throw syntax_error{error_messages::err_msg_1_8_6.message,
                       error_messages::err_msg_1_8_6.category, ctx.identifier,
                       ctx.line_count, ctx.char_count};
  } else {
    std::variant<std::string, std::string::size_type> file_path_escaped{
        replace_escape_sequences(file_path_str)};

    switch (file_path_escaped.index()) {
    case 0: {
      std::filesystem::path const file_path{
          std::get<std::string>(std::move(file_path_escaped))};
      if (file_path.is_absolute()) {
        return parser::parse_file(file_path);
      } else if (ctx.identifier_is_file_path == true) {
        return parser::parse_file(std::filesystem::path{
            std::filesystem::path{ctx.identifier}.parent_path() / file_path});
      } else {
        return parser::parse_file(file_path);
      }
    } break;

    case 1: {
      const std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
          invalid_escape_sequence_pos_count{
              start_of_file_path_str_pos_count.first,
              static_cast<decltype(ctx.char_count)>(
                  start_of_file_path_str_pos_count.second +
                  std::get<std::string::size_type>(file_path_escaped))};

      throw syntax_error{error_messages::err_msg_1_8_5.message,
                         error_messages::err_msg_1_8_5.category, ctx.identifier,
                         invalid_escape_sequence_pos_count.first,
                         invalid_escape_sequence_pos_count.second};
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
  char peek_char{static_cast<char>(ctx.input_stream.peek())};

  switch (peek_char) {

  case character_constants::g_k_comment_script: {
    ctx.input_stream.get(cur_char);
    ++ctx.char_count;

    while (true) {
      ctx.input_stream.get(cur_char);
      ++ctx.char_count;
      if (ctx.input_stream.eof() == true) {
        return true;
      } else if (cur_char == character_constants::g_k_newline) {
        ctx.input_stream.unget();
        return true;
      }
    }
  } break;

  case c_or_cpp_comment_leader: {
    ctx.input_stream.get(cur_char);
    ++ctx.char_count;
    peek_char = (ctx.input_stream.peek());

    switch (peek_char) {
    case character_constants::g_k_comment_cpp.back(): {
      ctx.input_stream.get(cur_char);
      ++ctx.char_count;

      while (true) {
        ctx.input_stream.get(cur_char);
        ++ctx.char_count;
        if (ctx.input_stream.eof() == true) {
          return true;
        } else if (cur_char == character_constants::g_k_newline) {
          ctx.input_stream.unget();
          return true;
        }
      }

    } break;

    case character_constants::g_k_comment_c_start.back(): {
      ctx.input_stream.get(cur_char);
      ++ctx.char_count;
      while (true) {
        ctx.input_stream.get(cur_char);
        ++ctx.char_count;
        if (ctx.input_stream.eof() == true) {
          throw syntax_error{error_messages::err_msg_1_1_1.message,
                             error_messages::err_msg_1_1_1.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        } else if (cur_char == character_constants::g_k_newline) {
          ++ctx.line_count;
          ctx.char_count = 0;
        } else if (cur_char == character_constants::g_k_comment_c_end.front()) {
          if (static_cast<char>(ctx.input_stream.peek()) ==
              character_constants::g_k_comment_c_end.back()) {
            ctx.input_stream.get();
            ++ctx.char_count;
            return true;
          }
        }
      }
    } break;

    default: {
      ctx.input_stream.unget();
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
  ctx.input_stream.get(escape_leader_char);
  if (ctx.input_stream.eof() == false) {
    ++ctx.char_count;
  }

  if (escape_leader_char == character_constants::g_k_escape_leader) {
    char escape_char_1{};
    ctx.input_stream.get(escape_char_1);

    if (ctx.input_stream.eof() == true) {
      throw syntax_error{error_messages::err_msg_1_9_1.message,
                         error_messages::err_msg_1_9_1.category, ctx.identifier,
                         ctx.line_count, ctx.char_count};
    } else if (escape_char_1 == character_constants::g_k_newline) {
      throw syntax_error{error_messages::err_msg_1_9_1.message,
                         error_messages::err_msg_1_9_1.category, ctx.identifier,
                         ctx.line_count, ctx.char_count};
    } else {
      ++ctx.char_count;

      if (escape_char_1 == character_constants::g_k_hex_escape_char) {
        char hex_digit_1{};
        ctx.input_stream.get(hex_digit_1);
        if (ctx.input_stream.eof() == true) {
          throw syntax_error{error_messages::err_msg_1_9_1.message,
                             error_messages::err_msg_1_9_1.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        } else if (hex_digit_1 == character_constants::g_k_newline) {
          throw syntax_error{error_messages::err_msg_1_9_1.message,
                             error_messages::err_msg_1_9_1.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        } else {
          ++ctx.char_count;
        }

        char hex_digit_2{};
        ctx.input_stream.get(hex_digit_2);
        if (ctx.input_stream.eof() == true) {
          throw syntax_error{error_messages::err_msg_1_9_1.message,
                             error_messages::err_msg_1_9_1.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        } else if (hex_digit_2 == character_constants::g_k_newline) {
          throw syntax_error{error_messages::err_msg_1_9_1.message,
                             error_messages::err_msg_1_9_1.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        } else {
          ++ctx.char_count;
        }

        if ((is_digit(hex_digit_1, numeral_system_hexadecimal)) &&
            (is_digit(hex_digit_2, numeral_system_hexadecimal))) {
          std::string const hex_string{std::string{} + hex_digit_1 +
                                       hex_digit_2};
          int ret_val{};
          std::from_chars(hex_string.data(),
                          hex_string.data() + hex_string.size(), ret_val,
                          numeral_system_hexadecimal.base);
          return static_cast<char>(ret_val);
        } else {
          throw syntax_error{error_messages::err_msg_1_9_4.message,
                             error_messages::err_msg_1_9_4.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        }
      } else {
        if (character_constants::g_k_basic_escape_chars.contains(
                escape_char_1)) {
          return character_constants::g_k_basic_escape_chars.at(escape_char_1);
        } else {
          throw syntax_error{error_messages::err_msg_1_9_2.message,
                             error_messages::err_msg_1_9_2.category,
                             ctx.identifier, ctx.line_count, ctx.char_count};
        }
      }
    }
  } else {
    throw syntax_error{error_messages::err_msg_1_9_3.message,
                       error_messages::err_msg_1_9_3.category, ctx.identifier,
                       ctx.line_count, ctx.char_count};
  }
}

libconfigfile::node_type
libconfigfile::parser::impl::identify_key_value_value_type(
    context &ctx, const std::string &possible_terminating_chars,
    char *actual_terminating_char /*= nullptr*/) {

  std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
      pos_count_at_start{ctx.line_count, ctx.char_count};

  std::string gotten_chars{};
  char cur_char{};
  bool eof{false};

  const auto reset_context{[&ctx, &gotten_chars, &pos_count_at_start]() {
    if (gotten_chars.empty() == false) {
      for (auto i{gotten_chars.rbegin()}; i != gotten_chars.rend(); ++i) {
        ctx.input_stream.putback(*i);
      }
    }
    ctx.line_count = pos_count_at_start.first;
    ctx.char_count = pos_count_at_start.second;
  }};

  while (true) {
    while (true) {
      handle_comments(ctx);
      ctx.input_stream.get(cur_char);
      if (ctx.input_stream.eof() == true) {
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
      const std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
          pos_count_at_error{ctx.line_count, ctx.char_count};

      reset_context();

      throw syntax_error{error_messages::err_msg_1_2_5.message,
                         error_messages::err_msg_1_2_5.category, ctx.identifier,
                         ctx.line_count, ctx.char_count};
    } else {
      gotten_chars.push_back(cur_char);

      if (is_whitespace(cur_char) == true) {
        continue;
      } else if (cur_char == character_constants::g_k_key_value_terminate) {
        const std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
            pos_count_at_error{ctx.line_count, ctx.char_count};

        reset_context();

        throw syntax_error{error_messages::err_msg_1_2_5.message,
                           error_messages::err_msg_1_2_5.category,
                           ctx.identifier, pos_count_at_start.first,
                           pos_count_at_error.second};
      } else {
        reset_context();

        switch (cur_char) {
        case character_constants::g_k_map_opening_delimiter: {
          return node_type::Map;
        } break;

        case character_constants::g_k_array_opening_delimiter: {
          return node_type::Array;
        } break;

        case character_constants::g_k_string_delimiter: {
          return node_type::String;
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

libconfigfile::node_type
libconfigfile::parser::impl::identify_key_value_numeric_value_type(
    context &ctx, const std::string &possible_terminating_chars,
    char *actual_terminating_char /*= nullptr*/) {

  std::string gotten_chars{};
  std::pair<decltype(ctx.line_count), decltype(ctx.char_count)>
      pos_count_at_start{ctx.line_count, ctx.char_count};
  char cur_char{};
  while (true) {
    handle_comments(ctx);
    ctx.input_stream.get(cur_char);
    if (ctx.input_stream.eof() == true) {
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
    if (gotten_chars.empty() == false) {
      for (auto i{gotten_chars.rbegin()}; i != gotten_chars.rend(); ++i) {
        ctx.input_stream.putback(*i);
      }
    }
    ctx.line_count = pos_count_at_start.first;
    ctx.char_count = pos_count_at_start.second;
  }};

  const auto cleanup_and_return{[&reset_context](const node_type ret_val) {
    reset_context();
    return ret_val;
  }};

  if ((case_insensitive_string_find(
           gotten_chars, character_constants::g_k_float_infinity.second) !=
       (std::string::npos)) ||
      (case_insensitive_string_find(
           gotten_chars, character_constants::g_k_float_not_a_number.second) !=
       (std::string::npos))) {
    return cleanup_and_return(node_type::Float);
  } else {
    if ((gotten_chars.find(character_constants::g_k_float_decimal_point)) !=
        (std::string::npos)) {
      return cleanup_and_return(node_type::Float);
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
          return cleanup_and_return(node_type::Integer);
        } else {
          return cleanup_and_return(node_type::Float);
        }
      } else {
        return cleanup_and_return(node_type::Integer);
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
      std::string::size_type const escape_char_pos_count{cur_char + 1};
      if (escape_char_pos_count < str.size()) {
        const char escape_char{str[escape_char_pos_count]};
        if (escape_char == character_constants::g_k_hex_escape_char) {
          std::string::size_type const hex_digit_pos_count_1{
              escape_char_pos_count + 1};
          std::string::size_type const hex_digit_pos_count_2{
              escape_char_pos_count + 2};
          if ((hex_digit_pos_count_1 < str.size()) &&
              (hex_digit_pos_count_2 < str.size())) {
            const char hex_digit_1{str[hex_digit_pos_count_1]};
            const char hex_digit_2{str[hex_digit_pos_count_2]};
            if ((is_digit(hex_digit_1, numeral_system_hexadecimal)) &&
                (is_digit(hex_digit_2, numeral_system_hexadecimal))) {
              std::string const hex_string{std::string{} + hex_digit_1 +
                                           hex_digit_2};
              int new_char{};
              std::from_chars(hex_string.data(),
                              hex_string.data() + hex_string.size(), new_char,
                              numeral_system_hexadecimal.base);
              result.push_back(static_cast<char>(new_char));
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
            cur_char = escape_char_pos_count;
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

bool libconfigfile::parser::impl::is_whitespace(
    const char ch,
    const std::string
        &whitespace_chars /*= character_constants::g_k_whitespace_chars*/) {
  return ((whitespace_chars.find(ch)) != (std::string::npos));
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
    const char ch,
    const numeral_system &num_sys /*= character_constants::g_k_dec_num_sys*/) {
  return (num_sys.digits.find(ch) != std::string::npos);
}

bool libconfigfile::parser::impl::case_insensitive_char_compare(
    const char ch1, const char ch2) {
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
