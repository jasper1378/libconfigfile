#ifndef LIBCONFIGFILE_NODE_PTR_HPP
#define LIBCONFIGFILE_NODE_PTR_HPP

#include "node.hpp"

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>

namespace libconfigfile {
template <typename node_t>
concept is_node_type = std::derived_from<node_t, node>;

template <is_node_type node_t> class node_ptr {
  template <is_node_type node_other_t> friend class node_ptr;

public:
  using ptr_t = node_t *;

private:
  ptr_t m_ptr;

public:
  node_ptr() : m_ptr{nullptr} {}

  node_ptr(std::nullptr_t) : m_ptr{nullptr} {}

  explicit node_ptr(ptr_t ptr) : m_ptr{ptr} {}

  node_ptr(const node_ptr &other) : m_ptr{other.m_ptr->create_clone()} {}

  node_ptr(node_ptr &&other) noexcept : m_ptr{other.m_ptr} {
    other.m_ptr = nullptr;
  }

  ~node_ptr() {
    delete m_ptr;
    m_ptr = nullptr;
  }

public:
  ptr_t release() {
    ptr_t temp{m_ptr};
    m_ptr = nullptr;
    return temp;
  }

  void reset(ptr_t ptr = nullptr) {
    ptr_t old_ptr{m_ptr};
    m_ptr = ptr;

    delete old_ptr;
  }

  void swap(node_ptr &other) {
    using std::swap;
    swap(m_ptr, other.m_ptr);
  }

  ptr_t get() const { return m_ptr; }

  template <is_node_type cast_t> node_ptr<cast_t> cast() const {
    return node_ptr<cast_t>{dynamic_cast<decltype(node_ptr<cast_t>{}.m_ptr)>(
        m_ptr->create_clone())};
  }

public:
  node_ptr &operator=(const node_ptr &other) {
    if (this == &other) {
      return *this;
    }

    reset(other.m_ptr->create_clone());

    return *this;
  }

  node_ptr &operator=(node_ptr &&other) noexcept {
    if (this == &other) {
      return *this;
    }

    reset(other.m_ptr);
    other.m_ptr = nullptr;

    return *this;
  }

  node_ptr &operator=(ptr_t ptr) {
    reset(ptr);

    return *this;
  }

  explicit operator bool() const { return m_ptr != nullptr; }

  ptr_t operator->() const { return m_ptr; }

public:
  template <is_node_type node_t1, is_node_type node_t2>
  friend bool operator==(const node_ptr<node_t1> &x,
                         const node_ptr<node_t2> &y);
  template <is_node_type node_t1, is_node_type node_t2>
  friend bool operator!=(const node_ptr<node_t1> &x,
                         const node_ptr<node_t2> &y);
  template <is_node_type node_t1, is_node_type node_t2>
  friend bool operator<(const node_ptr<node_t1> &x, const node_ptr<node_t2> &y);
  template <is_node_type node_t1, is_node_type node_t2>
  friend bool operator<=(const node_ptr<node_t1> &x,
                         const node_ptr<node_t2> &y);
  template <is_node_type node_t1, is_node_type node_t2>
  friend bool operator>(const node_ptr<node_t1> &x, const node_ptr<node_t2> &y);
  template <is_node_type node_t1, is_node_type node_t2>
  friend bool operator>=(const node_ptr<node_t1> &x,
                         const node_ptr<node_t2> &y);
  template <is_node_type node_t1, is_node_type node_t2>
    requires std::three_way_comparable_with<typename node_ptr<node_t1>::ptr_t,
                                            typename node_ptr<node_t2>::ptr_t>
  friend std::compare_three_way_result_t<typename node_ptr<node_t1>::ptr_t,
                                         typename node_ptr<node_t2>::ptr_t>
  operator<=>(const node_ptr<node_t1> &x, const node_ptr<node_t2> &y);
  template <is_node_type node_t1>
  friend bool operator==(const node_ptr<node_t1> &x, std::nullptr_t);
  template <is_node_type node_t1>
  friend bool operator==(std::nullptr_t, const node_ptr<node_t1> &x);
  template <is_node_type node_t1>
  friend bool operator!=(const node_ptr<node_t1> &x, std::nullptr_t);
  template <is_node_type node_t1>
  friend bool operator!=(std::nullptr_t, const node_ptr<node_t1> &x);
  template <is_node_type node_t1>
  friend bool operator<(const node_ptr<node_t1> &x, std::nullptr_t);
  template <is_node_type node_t1>
  friend bool operator<(std::nullptr_t, const node_ptr<node_t1> &y);
  template <is_node_type node_t1>
  friend bool operator<=(const node_ptr<node_t1> &x, std::nullptr_t);
  template <is_node_type node_t1>
  friend bool operator<=(std::nullptr_t, const node_ptr<node_t1> &y);
  template <is_node_type node_t1>
  friend bool operator>(const node_ptr<node_t1> &x, std::nullptr_t);
  template <is_node_type node_t1>
  friend bool operator>(std::nullptr_t, const node_ptr<node_t1> &y);
  template <is_node_type node_t1>
  friend bool operator>=(const node_ptr<node_t1> &x, std::nullptr_t);
  template <is_node_type node_t1>
  friend bool operator>=(std::nullptr_t, const node_ptr<node_t1> &y);
  template <is_node_type node_t1>
    requires std::three_way_comparable<typename node_ptr<node_t1>::ptr_t>
  friend std::compare_three_way_result_t<typename node_ptr<node_t1>::ptr_t>
  operator<=>(const node_ptr<node_t1> &x, std::nullptr_t);
};

template <is_node_type node_t, typename... args_t>
node_ptr<node_t> make_node_ptr(args_t &&...args) {
  return node_ptr<node_t>{new node_t{std::forward<args_t>(args)...}};
}

template <is_node_type node_t1, is_node_type node_t2>
bool operator==(const node_ptr<node_t1> &x, const node_ptr<node_t2> &y) {
  return x.get() == y.get();
}

template <is_node_type node_t1, is_node_type node_t2>
bool operator!=(const node_ptr<node_t1> &x, const node_ptr<node_t2> &y) {
  return x.get() != y.get();
}

template <is_node_type node_t1, is_node_type node_t2>
bool operator<(const node_ptr<node_t1> &x, const node_ptr<node_t2> &y) {
  using CT = typename std::common_type<typename node_ptr<node_t1>::ptr_t,
                                       typename node_ptr<node_t2>::ptr>::value;
  return std::less<CT>()(x.get(), y.get());
}

template <is_node_type node_t1, is_node_type node_t2>
bool operator<=(const node_ptr<node_t1> &x, const node_ptr<node_t2> &y) {
  return !(y < x);
}

template <is_node_type node_t1, is_node_type node_t2>
bool operator>(const node_ptr<node_t1> &x, const node_ptr<node_t2> &y) {
  return y < x;
}

template <is_node_type node_t1, is_node_type node_t2>
bool operator>=(const node_ptr<node_t1> &x, const node_ptr<node_t2> &y) {
  return !(x < y);
}

template <is_node_type node_t1, is_node_type node_t2>
  requires std::three_way_comparable_with<typename node_ptr<node_t1>::ptr_t,
                                          typename node_ptr<node_t2>::ptr_t>
std::compare_three_way_result_t<typename node_ptr<node_t1>::ptr_t,
                                typename node_ptr<node_t2>::ptr_t>
operator<=>(const node_ptr<node_t1> &x, const node_ptr<node_t2> &y) {

  return std::compare_three_way{}(x.get(), y.get());
}

template <is_node_type node_t1>
bool operator==(const node_ptr<node_t1> &x, std::nullptr_t) {
  return !x;
}

template <is_node_type node_t1>
bool operator==(std::nullptr_t, const node_ptr<node_t1> &x) {
  return !x;
}

template <is_node_type node_t1>
bool operator!=(const node_ptr<node_t1> &x, std::nullptr_t) {
  return (bool)x;
}

template <is_node_type node_t1>
bool operator!=(std::nullptr_t, const node_ptr<node_t1> &x) {
  return (bool)x;
}

template <is_node_type node_t1>
bool operator<(const node_ptr<node_t1> &x, std::nullptr_t) {
  return std::less<typename node_ptr<node_t1>::ptr_t>()(x.get(), nullptr);
}

template <is_node_type node_t1>
bool operator<(std::nullptr_t, const node_ptr<node_t1> &y) {
  return std::less<typename node_ptr<node_t1>::ptr_t>()(nullptr, y.get());
}

template <is_node_type node_t1>
bool operator<=(const node_ptr<node_t1> &x, std::nullptr_t) {
  return !(nullptr < x);
}

template <is_node_type node_t1>
bool operator<=(std::nullptr_t, const node_ptr<node_t1> &y) {
  return !(y < nullptr);
}

template <is_node_type node_t1>
bool operator>(const node_ptr<node_t1> &x, std::nullptr_t) {
  return nullptr < x;
}

template <is_node_type node_t1>
bool operator>(std::nullptr_t, const node_ptr<node_t1> &y) {
  return y < nullptr;
}

template <is_node_type node_t1>
bool operator>=(const node_ptr<node_t1> &x, std::nullptr_t) {
  return !(x < nullptr);
}

template <is_node_type node_t1>
bool operator>=(std::nullptr_t, const node_ptr<node_t1> &y) {
  return !(nullptr < y);
}

template <is_node_type node_t1>
  requires std::three_way_comparable<typename node_ptr<node_t1>::ptr_t>
std::compare_three_way_result_t<typename node_ptr<node_t1>::ptr_t>
operator<=>(const node_ptr<node_t1> &x, std::nullptr_t) {
  return std::compare_three_way{}(
      x.get(), static_cast<typename node_ptr<node_t1>::ptr_t>(nullptr));
}

} // namespace libconfigfile

#endif

// friend bool operator==(const node_ptr &first, const node_ptr &second);
//
// friend bool operator==(const node_ptr &first, std::nullptr_t);
//
// friend bool operator==(std::nullptr_t, const node_ptr &second);
//
// friend bool operator!=(const node_ptr &first, const node_ptr &second);
//
// friend bool operator!=(const node_ptr &first, std::nullptr_t);
//
// friend bool operator!=(std::nullptr_t, const node_ptr &second);

// template <is_node_type node_t>
// bool operator==(const node_ptr<node_t> &first, const node_ptr<node_t>
// &second) {
//   return first.m_ptr == second.m_ptr;
// }
//
// template <is_node_type node_t>
// bool operator==(const node_ptr<node_t> &first, std::nullptr_t) {
//   return first.m_ptr == nullptr;
// }
//
// template <is_node_type node_t>
// bool operator==(std::nullptr_t, const node_ptr<node_t> &second) {
//   return nullptr == second.m_ptr;
// }
//
// template <is_node_type node_t>
// bool operator!=(const node_ptr<node_t> &first, const node_ptr<node_t>
// &second) {
//   return first.m_ptr != second.m_ptr;
// }
//
// template <is_node_type node_t>
// bool operator!=(const node_ptr<node_t> &first, std::nullptr_t) {
//   return first.m_ptr != nullptr;
// }
//
// template <is_node_type node_t>
// bool operator!=(std::nullptr_t, const node_ptr<node_t> &second) {
//   return nullptr != second.m_ptr;
// }
