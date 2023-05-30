#ifndef LIBCONFIGFILE_NUMERAL_SYSTEM_HPP
#define LIBCONFIGFILE_NUMERAL_SYSTEM_HPP

#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>

namespace libconfigfile {
class numeral_system {
public:
  int base;
  char prefix;
  char prefix_alt;
  std::string_view digits;

public:
  constexpr ~numeral_system(){};

public:
  constexpr numeral_system(int a_base, char a_prefix, char a_prefix_alt,
                           std::string_view a_digits)
      : base{a_base}, prefix{a_prefix},
        prefix_alt{a_prefix_alt}, digits{a_digits} {}

  constexpr numeral_system(const numeral_system &other)
      : base{other.base}, prefix{other.prefix},
        prefix_alt{other.prefix_alt}, digits{other.digits} {}

  constexpr numeral_system(numeral_system &&other) noexcept(
      std::is_nothrow_move_constructible_v<decltype(base)>
          &&std::is_nothrow_move_constructible_v<decltype(prefix)>
              &&std::is_nothrow_move_constructible_v<decltype(prefix_alt)>
                  &&std::is_nothrow_move_constructible_v<decltype(digits)>)
      : base{other.base}, prefix{other.prefix},
        prefix_alt{other.prefix_alt}, digits{std::move(other.digits)} {}

private:
  constexpr numeral_system &operator=(const numeral_system &other) {
    base = other.base;
    prefix = other.prefix;
    prefix_alt = other.prefix_alt;
    digits = other.digits;
    return *this;
  }

  constexpr numeral_system &operator=(numeral_system &&other) noexcept(
      std::is_nothrow_move_assignable_v<decltype(base)>
          &&std::is_nothrow_move_assignable_v<decltype(prefix)>
              &&std::is_nothrow_move_assignable_v<decltype(prefix_alt)>
                  &&std::is_nothrow_move_assignable_v<decltype(digits)>) {
    base = other.base;
    prefix = other.prefix;
    prefix_alt = other.prefix_alt;
    digits = other.digits;
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

static constexpr numeral_system numeral_system_decimal{10, '\0', '\0',
                                                       "0123456789"};
static constexpr numeral_system numeral_system_binary{2, 'b', 'B', "01"};
static constexpr numeral_system numeral_system_octal{8, 'o', 'O', "01234567"};
static constexpr numeral_system numeral_system_hexadecimal{
    16, 'x', 'X', "0123456789abcdefABCDEF"};
} // namespace libconfigfile

#endif
