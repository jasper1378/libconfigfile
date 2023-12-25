#ifndef LIBCONFIGFILE_NUMERAL_SYSTEM_HPP
#define LIBCONFIGFILE_NUMERAL_SYSTEM_HPP

#include <string_view>
#include <type_traits>
#include <utility>

namespace libconfigfile {
class numeral_system {
public:
  int base;
  char prefix;
  char prefix_alt;
  std::string_view digits;
  bool (*is_digit)(const char ch);

public:
  constexpr ~numeral_system(){};

public:
  constexpr numeral_system(const int a_base, const char a_prefix,
                           const char a_prefix_alt, std::string_view a_digits,
                           bool (*is_digit)(const char ch))
      : base{a_base}, prefix{a_prefix}, prefix_alt{a_prefix_alt},
        digits{a_digits}, is_digit{is_digit} {}

  constexpr numeral_system(const numeral_system &other)
      : base{other.base}, prefix{other.prefix}, prefix_alt{other.prefix_alt},
        digits{other.digits} {}

  constexpr numeral_system(numeral_system &&other) noexcept(
      std::is_nothrow_move_constructible_v<decltype(base)>
          &&std::is_nothrow_move_constructible_v<decltype(prefix)>
              &&std::is_nothrow_move_constructible_v<decltype(prefix_alt)>
                  &&std::is_nothrow_move_constructible_v<decltype(digits)>
                      &&std::is_nothrow_move_assignable_v<decltype(is_digit)>)
      : base{other.base}, prefix{other.prefix}, prefix_alt{other.prefix_alt},
        digits{std::move(other.digits)} {}

private:
  constexpr numeral_system &operator=(const numeral_system &other) {
    base = other.base;
    prefix = other.prefix;
    prefix_alt = other.prefix_alt;
    digits = other.digits;
    is_digit = other.is_digit;
    return *this;
  }

  constexpr numeral_system &operator=(numeral_system &&other) noexcept(
      std::is_nothrow_move_assignable_v<decltype(base)>
          &&std::is_nothrow_move_assignable_v<decltype(prefix)>
              &&std::is_nothrow_move_assignable_v<decltype(prefix_alt)>
                  &&std::is_nothrow_move_assignable_v<decltype(digits)>
                      &&std::is_nothrow_move_assignable_v<decltype(is_digit)>) {
    base = other.base;
    prefix = other.prefix;
    prefix_alt = other.prefix_alt;
    digits = other.digits;
    is_digit = std::move(other.is_digit);
    return *this;
  }

public:
  friend constexpr bool operator==(const numeral_system &x,
                                   const numeral_system &y);
  friend constexpr bool operator!=(const numeral_system &x,
                                   const numeral_system &y);
};

bool constexpr operator==(const numeral_system &x, const numeral_system &y) {
  return ((x.base == y.base) && (x.prefix == y.prefix) &&
          (x.prefix_alt == y.prefix_alt) && (x.digits == y.digits));
}

bool constexpr operator!=(const numeral_system &x, const numeral_system &y) {
  return (!(x == y));
}

static constexpr numeral_system numeral_system_decimal{
    10, '\0', '\0', "0123456789", [](const char ch) -> bool {
      if (ch >= '0' && ch <= '9') {
        return true;
      } else {
        return false;
      }
    }};
static constexpr numeral_system numeral_system_binary{
    2, 'b', 'B', "01", [](const char ch) -> bool {
      if (ch == '0' || ch == '1') {
        return true;
      } else {
        return false;
      }
    }};
static constexpr numeral_system numeral_system_octal{
    8, 'o', 'O', "01234567", [](const char ch) -> bool {
      if (ch >= '0' && ch <= '7') {
        return true;
      } else {
        return false;
      }
    }};
static constexpr numeral_system numeral_system_hexadecimal{
    16, 'x', 'X', "0123456789abcdefABCDEF", [](const char ch) -> bool {
      if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') ||
          (ch >= 'A' && ch <= 'F')) {
        return true;
      } else {
        return false;
      }
    }};
} // namespace libconfigfile

#endif
