#ifndef BITS_AND_BYTES_DEFINE_ENUM_FLAG_OPERATORS_HPP
#define BITS_AND_BYTES_DEFINE_ENUM_FLAG_OPERATORS_HPP

#include <type_traits>

#define BITS_AND_BYTES_DEFINE_ENUM_FLAG_OPERATORS_FOR_TYPE(T)                  \
                                                                               \
  static_assert(std::is_enum_v<T>);                                            \
                                                                               \
  constexpr inline T operator~(const T a) {                                    \
    return static_cast<T>(~static_cast<std::underlying_type_t<T>>(a));         \
  }                                                                            \
                                                                               \
  constexpr inline T operator&(const T a, const T b) {                         \
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(a) &          \
                          static_cast<std::underlying_type_t<T>>(b));          \
  }                                                                            \
                                                                               \
  constexpr inline T operator|(const T a, const T b) {                         \
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(a) |          \
                          static_cast<std::underlying_type_t<T>>(b));          \
  }                                                                            \
                                                                               \
  constexpr inline T operator^(const T a, const T b) {                         \
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(a) ^          \
                          static_cast<std::underlying_type_t<T>>(b));          \
  }                                                                            \
                                                                               \
  constexpr inline T &operator&=(T &a, const T b) { return (a = (a & b)); }    \
                                                                               \
  constexpr inline T &operator|=(T &a, const T b) { return (a = (a | b)); }    \
                                                                               \
  constexpr inline T &operator^=(T &a, const T b) { return (a = (a ^ b)); }

#define BITS_AND_BYTES_DEFINE_ENUM_FLAG_OPERATORS_FOR_ALL()                    \
                                                                               \
  template <typename T>                                                        \
    requires std::is_enum_v<T>                                                 \
  constexpr inline T operator~(const T a) {                                    \
    return static_cast<T>(~static_cast<std::underlying_type_t<T>>(a));         \
  }                                                                            \
                                                                               \
  template <typename T>                                                        \
    requires std::is_enum_v<T>                                                 \
  constexpr inline T operator&(const T a, const T b) {                         \
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(a) &          \
                          static_cast<std::underlying_type_t<T>>(b));          \
  }                                                                            \
                                                                               \
  template <typename T>                                                        \
    requires std::is_enum_v<T>                                                 \
  constexpr inline T operator|(const T a, const T b) {                         \
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(a) |          \
                          static_cast<std::underlying_type_t<T>>(b));          \
  }                                                                            \
                                                                               \
  template <typename T>                                                        \
    requires std::is_enum_v<T>                                                 \
  constexpr inline T operator^(const T a, const T b) {                         \
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(a) ^          \
                          static_cast<std::underlying_type_t<T>>(b));          \
  }                                                                            \
                                                                               \
  template <typename T>                                                        \
    requires std::is_enum_v<T>                                                 \
  constexpr inline T &operator&=(T &a, const T b) {                            \
    return (a = (a & b));                                                      \
  }                                                                            \
                                                                               \
  template <typename T>                                                        \
    requires std::is_enum_v<T>                                                 \
  constexpr inline T &operator|=(T &a, const T b) {                            \
    return (a = (a | b));                                                      \
  }                                                                            \
                                                                               \
  template <typename T>                                                        \
    requires std::is_enum_v<T>                                                 \
  constexpr inline T &operator^=(T &a, const T b) {                            \
    return (a = (a ^ b));                                                      \
  }

#endif
