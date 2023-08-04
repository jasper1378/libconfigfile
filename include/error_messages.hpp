#ifndef LIBCONFIGFILE_ERROR_MESSAGES_HPP
#define LIBCONFIGFILE_ERROR_MESSAGES_HPP

#include <string>

namespace libconfigfile {
namespace error_messages {
/*   digit 1:
 *   1 syntax
 *   2 semantic
 *
 *   digit 2:
 *   1 comment
 *   2 key-value
 *   3 string
 *   4 integer
 *   5 float
 *   6 array
 *   7 map
 *   8 directive
 *   9 misc
 *
 *   digit 3:
 *   individual */

// clang-format off
  static const std::string err_msg_1_1_1 {"/error/syntax/comment: C-style comment is unterminated"};

  static const std::string err_msg_1_2_1 {"/error/syntax/key-value: key-value key contains invalid character(s)"};
  static const std::string err_msg_1_2_2 {"/error/syntax/key-value: key-value key is split by comment(s)"};
  static const std::string err_msg_1_2_3 {"/error/syntax/key-value: key-value key is split by newline(s)"};
  static const std::string err_msg_1_2_4 {"/error/syntax/key-value: key is missing"};
  static const std::string err_msg_1_2_5 {"/error/syntax/key-value: value is missing"};
  static const std::string err_msg_1_2_6 {"/error/syntax/key-value: value is unterminated"};
  static const std::string err_msg_1_2_7 {"/error/syntax/key-value: key-value is missing"};

  static const std::string err_msg_1_3_1 {"/error/syntax/string: non-whitespace character appears outside of string"};
  static const std::string err_msg_1_3_2 {"/error/syntax/string: string is split by newline(s)"};
  static const std::string err_msg_1_3_3 {"/error/syntax/string: string is unterminated"};

  static const std::string err_msg_1_4_1 {"/error/syntax/integer: integer contains invalid character(s)"};
  static const std::string err_msg_1_4_2 {"/error/syntax/integer: integer digit separator is not surrounded by at least one digit on either side"};
  static const std::string err_msg_1_4_3 {"/error/syntax/integer: integer is split by comment(s)"};
  static const std::string err_msg_1_4_4 {"/error/syntax/integer: integer is split by newline(s)"};
  static const std::string err_msg_1_4_5 {"/error/syntax/integer: integer value is too large"};
  static const std::string err_msg_1_4_6 {"/error/syntax/integer: negative sign does not appear at start of integer"};
  static const std::string err_msg_1_4_7 {"/error/syntax/integer: numeral system prefix does not appear before integer digits"};
  static const std::string err_msg_1_4_8 {"/error/syntax/integer: positive sign does not appear at start of integer"};

  static const std::string err_msg_1_5_1 {"/error/syntax/float: float contains invalid character(s)"};
  static const std::string err_msg_1_5_2 {"/error/syntax/float: float contains more than one decimal point"};
  static const std::string err_msg_1_5_3 {"/error/syntax/float: float contains more than one exponent sign"};
  static const std::string err_msg_1_5_4 {"/error/syntax/float: float decimal point appears after exponent sign"};
  static const std::string err_msg_1_5_5 {"/error/syntax/float: float decimal point is not surrounded by at least one digit on either side"};
  static const std::string err_msg_1_5_6 {"/error/syntax/float: float digit separator is not surrounded by at least one digit on either side"};
  static const std::string err_msg_1_5_7 {"/error/syntax/float: float exponent sign is not surrounded by at least one digit on either side"};
  static const std::string err_msg_1_5_8 {"/error/syntax/float: float is split by comment(s)"};
  static const std::string err_msg_1_5_9 {"/error/syntax/float: float is split by newline(s)"};
  static const std::string err_msg_1_5_10 {"/error/syntax/float: float value in too large"};
  static const std::string err_msg_1_5_11 {"/error/syntax/float: negative sign does not appear at start of integer or exponent part of float"};
  static const std::string err_msg_1_5_12 {"/error/syntax/float: positive sign does not appear at start of integer or exponent part of float"};

  static const std::string err_msg_1_6_1{"/error/syntax/array: array opening delimiter is missing"};
  static const std::string err_msg_1_6_2{"/error/syntax/array: array closing delimiter is missing"};
  static const std::string err_msg_1_6_3{"/error/syntax/array: extraneous character(s) appear after array"};

  static const std::string err_msg_1_7_2{"/error/syntax/map: map closing delimiter is missing"};
  static const std::string err_msg_1_7_3{"/error/syntax/map: extranous character(s) appear after map"};

  static const std::string err_msg_1_8_1 {"/error/syntax/directive: directive is invalid"};
  static const std::string err_msg_1_8_2 {"/error/syntax/directive: directive is split by newline(s)"};
  static const std::string err_msg_1_8_3 {"/error/syntax/directive: directive name is missing"};
  static const std::string err_msg_1_8_4 {"/error/syntax/directive: directive name is split by comment(s)"};
  static const std::string err_msg_1_8_5 {"/error/syntax/directive: escape sequence in include directive file path argument is invalid"};
  static const std::string err_msg_1_8_6 {"/error/syntax/directive: include directive file path argument is empty"};
  static const std::string err_msg_1_8_7 {"/error/syntax/directive: include directive file path argument is missing" "include directive requires file path argument"};
  static const std::string err_msg_1_8_8 {"/error/syntax/directive: include directive file path argument is unterminated"};
  static const std::string err_msg_1_8_9 {"/error/syntax/directive: include directive given excess arguments"};
  static const std::string err_msg_1_8_10 {"/error/syntax/directive: parser and configuration file version are incompatible"};
  static const std::string err_msg_1_8_11 {"/error/syntax/directive: version directive given excess arguments"};
  static const std::string err_msg_1_8_12 {"/error/syntax/directive: version directive version argument is empty"};
  static const std::string err_msg_1_8_13 {"/error/syntax/directive: version directive version argument is missing"};
  static const std::string err_msg_1_8_14 {"/error/syntax/directive: version directive version argument is unterminated"};

  static const std::string err_msg_1_9_1 {"/error/syntax/misc: escape sequence is incomplete"};
  static const std::string err_msg_1_9_2 {"/error/syntax/misc: escape sequence is invalid"};
  static const std::string err_msg_1_9_3 {"/error/syntax/misc: escape sequence leader is missing"};
  static const std::string err_msg_1_9_4 {"/error/syntax/misc: hexadecimal escape sequence contains invalid digit"};
  static const std::string err_msg_1_9_5 {"/error/syntax/misc: duplicate name in scope"};
// clang-format on

} // namespace error_messages
} // namespace libconfigfile

#endif
