#ifndef LIBCONFGFILE_CONSTEXPR_TOLOWER_TOUPPER_HPP
#define LIBCONFGFILE_CONSTEXPR_TOLOWER_TOUPPER_HPP

namespace libconfigfile {
template <int ch> struct tolower_impl {
  static constexpr int result{ch};
};

template <> struct tolower_impl<static_cast<int>('A')> {
  static constexpr int result{static_cast<int>('a')};
};

template <> struct tolower_impl<static_cast<int>('B')> {
  static constexpr int result{static_cast<int>('b')};
};

template <> struct tolower_impl<static_cast<int>('C')> {
  static constexpr int result{static_cast<int>('c')};
};

template <> struct tolower_impl<static_cast<int>('D')> {
  static constexpr int result{static_cast<int>('d')};
};

template <> struct tolower_impl<static_cast<int>('E')> {
  static constexpr int result{static_cast<int>('e')};
};

template <> struct tolower_impl<static_cast<int>('F')> {
  static constexpr int result{static_cast<int>('f')};
};

template <> struct tolower_impl<static_cast<int>('G')> {
  static constexpr int result{static_cast<int>('g')};
};

template <> struct tolower_impl<static_cast<int>('H')> {
  static constexpr int result{static_cast<int>('h')};
};

template <> struct tolower_impl<static_cast<int>('I')> {
  static constexpr int result{static_cast<int>('i')};
};

template <> struct tolower_impl<static_cast<int>('J')> {
  static constexpr int result{static_cast<int>('j')};
};

template <> struct tolower_impl<static_cast<int>('K')> {
  static constexpr int result{static_cast<int>('k')};
};

template <> struct tolower_impl<static_cast<int>('L')> {
  static constexpr int result{static_cast<int>('l')};
};

template <> struct tolower_impl<static_cast<int>('M')> {
  static constexpr int result{static_cast<int>('m')};
};

template <> struct tolower_impl<static_cast<int>('N')> {
  static constexpr int result{static_cast<int>('n')};
};

template <> struct tolower_impl<static_cast<int>('O')> {
  static constexpr int result{static_cast<int>('o')};
};

template <> struct tolower_impl<static_cast<int>('P')> {
  static constexpr int result{static_cast<int>('p')};
};

template <> struct tolower_impl<static_cast<int>('Q')> {
  static constexpr int result{static_cast<int>('q')};
};

template <> struct tolower_impl<static_cast<int>('R')> {
  static constexpr int result{static_cast<int>('r')};
};

template <> struct tolower_impl<static_cast<int>('S')> {
  static constexpr int result{static_cast<int>('s')};
};

template <> struct tolower_impl<static_cast<int>('T')> {
  static constexpr int result{static_cast<int>('t')};
};

template <> struct tolower_impl<static_cast<int>('U')> {
  static constexpr int result{static_cast<int>('u')};
};

template <> struct tolower_impl<static_cast<int>('V')> {
  static constexpr int result{static_cast<int>('v')};
};

template <> struct tolower_impl<static_cast<int>('W')> {
  static constexpr int result{static_cast<int>('w')};
};

template <> struct tolower_impl<static_cast<int>('X')> {
  static constexpr int result{static_cast<int>('x')};
};

template <> struct tolower_impl<static_cast<int>('Y')> {
  static constexpr int result{static_cast<int>('y')};
};

template <> struct tolower_impl<static_cast<int>('Z')> {
  static constexpr int result{static_cast<int>('z')};
};

template <int ch> struct toupper_impl {
  static constexpr int result{ch};
};

template <> struct toupper_impl<static_cast<int>('a')> {
  static constexpr int result{static_cast<int>('A')};
};

template <> struct toupper_impl<static_cast<int>('b')> {
  static constexpr int result{static_cast<int>('B')};
};

template <> struct toupper_impl<static_cast<int>('c')> {
  static constexpr int result{static_cast<int>('C')};
};

template <> struct toupper_impl<static_cast<int>('d')> {
  static constexpr int result{static_cast<int>('D')};
};

template <> struct toupper_impl<static_cast<int>('e')> {
  static constexpr int result{static_cast<int>('E')};
};

template <> struct toupper_impl<static_cast<int>('f')> {
  static constexpr int result{static_cast<int>('F')};
};

template <> struct toupper_impl<static_cast<int>('g')> {
  static constexpr int result{static_cast<int>('G')};
};

template <> struct toupper_impl<static_cast<int>('h')> {
  static constexpr int result{static_cast<int>('H')};
};

template <> struct toupper_impl<static_cast<int>('i')> {
  static constexpr int result{static_cast<int>('I')};
};

template <> struct toupper_impl<static_cast<int>('j')> {
  static constexpr int result{static_cast<int>('J')};
};

template <> struct toupper_impl<static_cast<int>('k')> {
  static constexpr int result{static_cast<int>('K')};
};

template <> struct toupper_impl<static_cast<int>('l')> {
  static constexpr int result{static_cast<int>('L')};
};

template <> struct toupper_impl<static_cast<int>('m')> {
  static constexpr int result{static_cast<int>('M')};
};

template <> struct toupper_impl<static_cast<int>('n')> {
  static constexpr int result{static_cast<int>('N')};
};

template <> struct toupper_impl<static_cast<int>('o')> {
  static constexpr int result{static_cast<int>('O')};
};

template <> struct toupper_impl<static_cast<int>('p')> {
  static constexpr int result{static_cast<int>('P')};
};

template <> struct toupper_impl<static_cast<int>('q')> {
  static constexpr int result{static_cast<int>('Q')};
};

template <> struct toupper_impl<static_cast<int>('r')> {
  static constexpr int result{static_cast<int>('R')};
};

template <> struct toupper_impl<static_cast<int>('s')> {
  static constexpr int result{static_cast<int>('S')};
};

template <> struct toupper_impl<static_cast<int>('t')> {
  static constexpr int result{static_cast<int>('T')};
};

template <> struct toupper_impl<static_cast<int>('u')> {
  static constexpr int result{static_cast<int>('U')};
};

template <> struct toupper_impl<static_cast<int>('v')> {
  static constexpr int result{static_cast<int>('V')};
};

template <> struct toupper_impl<static_cast<int>('w')> {
  static constexpr int result{static_cast<int>('W')};
};

template <> struct toupper_impl<static_cast<int>('x')> {
  static constexpr int result{static_cast<int>('X')};
};

template <> struct toupper_impl<static_cast<int>('y')> {
  static constexpr int result{static_cast<int>('Y')};
};

template <> struct toupper_impl<static_cast<int>('Z')> {
  static constexpr int result{static_cast<int>('z')};
};

template <int ch> constexpr int tolower() { return tolower_impl<ch>::result; }

template <int ch> constexpr int toupper() { return toupper_impl<ch>::result; }
} // namespace libconfigfile

#endif
