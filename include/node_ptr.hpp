#ifndef LIBCONFIGFILE_NODE_PTR_HPP
#define LIBCONFIGFILE_NODE_PTR_HPP

#include "node.hpp"
#include "node_concepts.hpp"

#include <compare>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace libconfigfile {

template <is_node_type t_node, bool t_compare_equality_by_value = false>
class node_ptr {
  template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
  friend class node_ptr;

public:
  using t_ptr = t_node *;

private:
  t_ptr m_ptr;

public:
  node_ptr() : m_ptr{nullptr} {}

  node_ptr(std::nullptr_t) : m_ptr{nullptr} {}

  explicit node_ptr(t_ptr ptr) : m_ptr{ptr} {}

  node_ptr(const node_ptr &other) : m_ptr{other.get()->create_clone()} {}

  node_ptr(node_ptr &&other) noexcept : m_ptr{other.release()} {};

  ~node_ptr() {
    delete m_ptr;
    m_ptr = nullptr;
  }

public:
  t_ptr release() {
    t_ptr temp{m_ptr};
    m_ptr = nullptr;
    return temp;
  }

  void reset(t_ptr ptr = nullptr) {
    t_ptr old_ptr{m_ptr};
    m_ptr = ptr;
    delete old_ptr;
  }

  void swap(node_ptr &other) {
    using std::swap;
    swap(m_ptr, other.m_ptr);
  }

  t_ptr get() const { return m_ptr; }

public:
  node_ptr &operator=(const node_ptr &other) {
    if (this == &other) {
      return *this;
    }

    reset(other.get()->create_clone());

    return *this;
  }

  node_ptr &operator=(node_ptr &&other) noexcept {
    if (this == &other) {
      return *this;
    }

    reset(other.release());

    return *this;
  }

  node_ptr &operator=(t_ptr ptr) {
    reset(ptr);

    return *this;
  }

  explicit operator bool() const { return m_ptr != nullptr; }

  t_node operator*() const { return *get(); };

  t_ptr operator->() const { return get(); }

  template <bool t_compare_equality_by_value_1>
  operator const node_ptr<t_node, t_compare_equality_by_value_1>() const & {
    return (
        node_ptr<t_node, t_compare_equality_by_value_1>{get()->create_clone()});
  }

  template <bool t_compare_equality_by_value_1>
  operator node_ptr<t_node, t_compare_equality_by_value_1>() && {
    return (node_ptr<t_node, t_compare_equality_by_value_1>{release()});
  }

public:
  template <is_node_type t_cast, bool t_compare_equality_by_value_1,
            is_node_type t_start, bool t_compare_equality_by_value_2>
  friend node_ptr<t_cast, t_compare_equality_by_value_1>
  node_ptr_cast(const node_ptr<t_start, t_compare_equality_by_value_2> &np);
  template <is_node_type t_cast, bool t_compare_equality_by_value_1,
            is_node_type t_start, bool t_compare_equality_by_value_2>
  friend node_ptr<t_cast, t_compare_equality_by_value_1>
  node_ptr_cast(node_ptr<t_start, t_compare_equality_by_value_2> &&np);

  template <is_node_type t_node_1, bool t_compare_equality_by_value_1,
            typename... t_args>
  friend node_ptr<t_node_1, t_compare_equality_by_value_1>
  make_node_ptr(t_args &&...args);

  template <is_node_type t_node_1, bool t_compare_equality_by_value_1,
            is_node_type t_node_2, bool t_compare_equality_by_value_2>
  friend bool node_ptr_addr_equal_to(
      const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
      const node_ptr<t_node_2, t_compare_equality_by_value_2> &y);
  template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
  friend bool node_ptr_addr_equal_to(
      const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
      std::nullptr_t);
  template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
  friend bool node_ptr_addr_equal_to(
      std::nullptr_t,
      const node_ptr<t_node_1, t_compare_equality_by_value_1> &x);

  template <is_node_type t_node_1, bool t_compare_equality_by_value_1,
            is_node_type t_node_2, bool t_compare_equality_by_value_2>
  friend bool node_ptr_addr_not_equal_to(
      const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
      const node_ptr<t_node_2, t_compare_equality_by_value_2> &y);
  template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
  friend bool node_ptr_addr_not_equal_to(
      const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
      std::nullptr_t);
  template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
  friend bool node_ptr_addr_not_equal_to(
      std::nullptr_t,
      const node_ptr<t_node_1, t_compare_equality_by_value_1> &x);

  template <is_node_type t_node_1, bool t_compare_equality_by_value_1,
            is_node_type t_node_2, bool t_compare_equality_by_value_2>
  friend bool node_ptr_addr_greater(
      const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
      const node_ptr<t_node_2, t_compare_equality_by_value_2> &y);
  template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
  friend bool node_ptr_addr_greater(
      const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
      std::nullptr_t);
  template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
  friend bool node_ptr_addr_greater(
      std::nullptr_t,
      const node_ptr<t_node_1, t_compare_equality_by_value_1> &x);

  template <is_node_type t_node_1, bool t_compare_equality_by_value_1,
            is_node_type t_node_2, bool t_compare_equality_by_value_2>
  friend bool node_ptr_addr_less(
      const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
      const node_ptr<t_node_2, t_compare_equality_by_value_2> &y);
  template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
  friend bool
  node_ptr_addr_less(const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
                     std::nullptr_t);
  template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
  friend bool node_ptr_addr_less(
      std::nullptr_t,
      const node_ptr<t_node_1, t_compare_equality_by_value_1> &x);

  template <is_node_type t_node_1, bool t_compare_equality_by_value_1,
            is_node_type t_node_2, bool t_compare_equality_by_value_2>
  friend bool node_ptr_addr_greater_equal(
      const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
      const node_ptr<t_node_2, t_compare_equality_by_value_2> &y);
  template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
  friend bool node_ptr_addr_greater_equal(
      const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
      std::nullptr_t);
  template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
  friend bool node_ptr_addr_greater_equal(
      std::nullptr_t,
      const node_ptr<t_node_1, t_compare_equality_by_value_1> &x);

  template <is_node_type t_node_1, bool t_compare_equality_by_value_1,
            is_node_type t_node_2, bool t_compare_equality_by_value_2>
  friend bool node_ptr_addr_less_equal(
      const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
      const node_ptr<t_node_2, t_compare_equality_by_value_2> &y);
  template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
  friend bool node_ptr_addr_less_equal(
      const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
      std::nullptr_t);
  template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
  friend bool node_ptr_addr_less_equal(
      std::nullptr_t,
      const node_ptr<t_node_1, t_compare_equality_by_value_1> &x);

  template <is_node_type t_node_1, bool t_compare_equality_by_value_1,
            is_node_type t_node_2, bool t_compare_equality_by_value_2>
    requires std::three_way_comparable<node *>
  std::
      compare_three_way_result_t<node *> friend node_ptr_addr_compare_three_way(
          const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
          const node_ptr<t_node_2, t_compare_equality_by_value_2> &y);
  template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
    requires std::three_way_comparable<node *>
  std::
      compare_three_way_result_t<node *> friend node_ptr_addr_compare_three_way(
          const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
          std::nullptr_t);
  template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
    requires std::three_way_comparable<node *>
  std::
      compare_three_way_result_t<node *> friend node_ptr_addr_compare_three_way(
          std::nullptr_t,
          const node_ptr<t_node_1, t_compare_equality_by_value_1> &x);

  template <is_node_type t_node_1, bool t_compare_equality_by_value_1,
            is_node_type t_node_2, bool t_compare_equality_by_value_2>
  friend bool node_ptr_val_equal_to(
      const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
      const node_ptr<t_node_2, t_compare_equality_by_value_2> &y);

  template <is_node_type t_node_1, bool t_compare_equality_by_value_1,
            is_node_type t_node_2, bool t_compare_equality_by_value_2>
  friend bool node_ptr_val_not_equal_to(
      const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
      const node_ptr<t_node_2, t_compare_equality_by_value_2> &y);

  template <is_node_type t_node_1, is_node_type t_node_2>
  friend bool operator==(const node_ptr<t_node_1, false> &x,
                         const node_ptr<t_node_2, false> &y);
  template <is_node_type t_node_1>
  friend bool operator==(const node_ptr<t_node_1, false> &x, std::nullptr_t);
  template <is_node_type t_node_1>
  friend bool operator==(std::nullptr_t, const node_ptr<t_node_1, false> &x);

  template <is_node_type t_node_1, is_node_type t_node_2>
  friend bool operator!=(const node_ptr<t_node_1, false> &x,
                         const node_ptr<t_node_2, false> &y);
  template <is_node_type t_node_1>
  friend bool operator!=(const node_ptr<t_node_1, false> &x, std::nullptr_t);
  template <is_node_type t_node_1>
  friend bool operator!=(std::nullptr_t, const node_ptr<t_node_1, false> &x);

  template <is_node_type t_node_1, is_node_type t_node_2>
  friend bool operator<(const node_ptr<t_node_1, false> &x,
                        const node_ptr<t_node_2, false> &y);
  template <is_node_type t_node_1>
  friend bool operator<(const node_ptr<t_node_1, false> &x, std::nullptr_t);
  template <is_node_type t_node_1>
  friend bool operator<(std::nullptr_t, const node_ptr<t_node_1, false> &x);

  template <is_node_type t_node_1, is_node_type t_node_2>
  friend bool operator<=(const node_ptr<t_node_1, false> &x,
                         const node_ptr<t_node_2, false> &y);
  template <is_node_type t_node_1>
  friend bool operator<=(const node_ptr<t_node_1, false> &x, std::nullptr_t);
  template <is_node_type t_node_1>
  friend bool operator<=(std::nullptr_t, const node_ptr<t_node_1, false> &x);

  template <is_node_type t_node_1, is_node_type t_node_2>
  friend bool operator>(const node_ptr<t_node_1, false> &x,
                        const node_ptr<t_node_2, false> &y);
  template <is_node_type t_node_1>
  friend bool operator>(const node_ptr<t_node_1, false> &x, std::nullptr_t);
  template <is_node_type t_node_1>
  friend bool operator>(std::nullptr_t, const node_ptr<t_node_1, false> &x);

  template <is_node_type t_node_1, is_node_type t_node_2>
  friend bool operator>=(const node_ptr<t_node_1, false> &x,
                         const node_ptr<t_node_2, false> &y);
  template <is_node_type t_node_1>
  friend bool operator>=(const node_ptr<t_node_1, false> &x, std::nullptr_t);
  template <is_node_type t_node_1>
  friend bool operator>=(std::nullptr_t, const node_ptr<t_node_1, false> &x);

  template <is_node_type t_node_1, bool t_compare_equality_by_value_1,
            is_node_type t_node_2, bool t_compare_equality_by_value_2>
    requires std::three_way_comparable<node *>
  std::compare_three_way_result_t<node *> friend
  operator<=>(const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
              const node_ptr<t_node_2, t_compare_equality_by_value_2> &y);
  template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
    requires std::three_way_comparable<node *>
  std::compare_three_way_result_t<node *> friend
  operator<=>(const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
              std::nullptr_t);
  template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
    requires std::three_way_comparable<node *>
  std::compare_three_way_result_t<node *> friend
  operator<=>(std::nullptr_t,
              const node_ptr<t_node_1, t_compare_equality_by_value_1> &x);

  template <is_node_type t_node_1, is_node_type t_node_2>
  friend bool operator==(const node_ptr<t_node_1, true> &x,
                         const node_ptr<t_node_2, true> &y);

  template <is_node_type t_node_1, is_node_type t_node_2>
  friend bool operator!=(const node_ptr<t_node_1, true> &x,
                         const node_ptr<t_node_2, true> &y);
};

template <is_node_type t_cast, bool t_compare_equality_by_value_1 = false,
          is_node_type t_start, bool t_compare_equality_by_value_2 = false>
node_ptr<t_cast, t_compare_equality_by_value_1>
node_ptr_cast(const node_ptr<t_start, t_compare_equality_by_value_2> &np) {
  return node_ptr<t_cast, t_compare_equality_by_value_1>{dynamic_cast<
      typename node_ptr<t_cast, t_compare_equality_by_value_1>::t_ptr>(
      np.get()->create_clone())};
}

template <is_node_type t_cast, bool t_compare_equality_by_value_1 = false,
          is_node_type t_start, bool t_compare_equality_by_value_2 = false>
node_ptr<t_cast, t_compare_equality_by_value_1>
node_ptr_cast(node_ptr<t_start, t_compare_equality_by_value_2> &&np) {
  return node_ptr<t_cast, t_compare_equality_by_value_1>{dynamic_cast<
      typename node_ptr<t_cast, t_compare_equality_by_value_1>::t_ptr>(
      np.release())};
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false,
          typename... t_args>
node_ptr<t_node_1, t_compare_equality_by_value_1>
make_node_ptr(t_args &&...args) {
  return node_ptr<t_node_1, t_compare_equality_by_value_1>{
      new t_node_1{std::forward<t_args>(args)...}};
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false,
          is_node_type t_node_2, bool t_compare_equality_by_value_2 = false>
bool node_ptr_addr_equal_to(
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
    const node_ptr<t_node_2, t_compare_equality_by_value_2> &y) {
  return ((static_cast<node *>(x.get())) == (static_cast<node *>(y.get())));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false>
bool node_ptr_addr_equal_to(
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
    std::nullptr_t) {
  return ((x.get()) == (nullptr));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false>
bool node_ptr_addr_equal_to(
    std::nullptr_t,
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x) {
  return ((nullptr) == (x.get()));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false,
          is_node_type t_node_2, bool t_compare_equality_by_value_2 = false>
bool node_ptr_addr_not_equal_to(
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
    const node_ptr<t_node_2, t_compare_equality_by_value_2> &y) {
  return ((static_cast<node *>(x.get())) != (static_cast<node *>(y.get())));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false>
bool node_ptr_addr_not_equal_to(
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
    std::nullptr_t) {
  return ((x.get()) != (nullptr));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false>
bool node_ptr_addr_not_equal_to(
    std::nullptr_t,
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x) {
  return ((nullptr) != (x.get()));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false,
          is_node_type t_node_2, bool t_compare_equality_by_value_2 = false>
bool node_ptr_addr_greater(
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
    const node_ptr<t_node_2, t_compare_equality_by_value_2> &y) {
  return ((static_cast<node *>(x.get())) > (static_cast<node *>(y.get())));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false>
bool node_ptr_addr_greater(
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
    std::nullptr_t) {
  return ((x.get()) > (nullptr));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false>
bool node_ptr_addr_greater(
    std::nullptr_t,
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x) {
  return ((nullptr) > (x.get()));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false,
          is_node_type t_node_2, bool t_compare_equality_by_value_2 = false>
bool node_ptr_addr_less(
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
    const node_ptr<t_node_2, t_compare_equality_by_value_2> &y) {
  return ((static_cast<node *>(x.get())) < (static_cast<node *>(y.get())));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false>
bool node_ptr_addr_less(
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
    std::nullptr_t) {
  return ((x.get()) < (nullptr));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false>
bool node_ptr_addr_less(
    std::nullptr_t,
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x) {
  return ((nullptr) < (x.get()));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false,
          is_node_type t_node_2, bool t_compare_equality_by_value_2 = false>
bool node_ptr_addr_greater_equal(
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
    const node_ptr<t_node_2, t_compare_equality_by_value_2> &y) {
  return ((static_cast<node *>(x.get())) >= (static_cast<node *>(y.get())));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false>
bool node_ptr_addr_greater_equal(
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
    std::nullptr_t) {
  return ((x.get()) >= (nullptr));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false>
bool node_ptr_addr_greater_equal(
    std::nullptr_t,
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x) {
  return ((nullptr) >= (x.get()));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false,
          is_node_type t_node_2, bool t_compare_equality_by_value_2 = false>
bool node_ptr_addr_less_equal(
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
    const node_ptr<t_node_2, t_compare_equality_by_value_2> &y) {
  return ((static_cast<node *>(x.get())) <= (static_cast<node *>(y.get())));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false>
bool node_ptr_addr_less_equal(
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
    std::nullptr_t) {
  return ((x.get()) <= (nullptr));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false>
bool node_ptr_addr_less_equal(
    std::nullptr_t,
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x) {
  return ((nullptr) <= (x.get()));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1,
          is_node_type t_node_2, bool t_compare_equality_by_value_2>
  requires std::three_way_comparable<node *>
std::compare_three_way_result_t<node *> node_ptr_addr_compare_three_way(
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
    const node_ptr<t_node_2, t_compare_equality_by_value_2> &y) {
  return (std::compare_three_way{}((static_cast<node *>(x.get())),
                                   (static_cast<node *>(y.get()))));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
  requires std::three_way_comparable<node *>
std::compare_three_way_result_t<node *> node_ptr_addr_compare_three_way(
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
    std::nullptr_t) {
  return (std::compare_three_way{}((static_cast<node *>(x.get())),
                                   (static_cast<node *>(nullptr))));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
  requires std::three_way_comparable<node *>
std::compare_three_way_result_t<node *> node_ptr_addr_compare_three_way(
    std::nullptr_t,
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x) {
  return (std::compare_three_way{}((static_cast<node *>(nullptr)),
                                   (static_cast<node *>(x.get()))));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false,
          is_node_type t_node_2, bool t_compare_equality_by_value_2 = false>
bool node_ptr_val_equal_to(
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
    const node_ptr<t_node_2, t_compare_equality_by_value_2> &y) {
  return (x.get()->polymorphic_value_compare(y.get()));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1 = false,
          is_node_type t_node_2, bool t_compare_equality_by_value_2 = false>
bool node_ptr_val_not_equal_to(
    const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
    const node_ptr<t_node_2, t_compare_equality_by_value_2> &y) {
  return (!(node_ptr_val_equal_to(x, y)));
}

template <is_node_type t_node_1, is_node_type t_node_2>
bool operator==(const node_ptr<t_node_1, false> &x,
                const node_ptr<t_node_2, false> &y) {
  return (node_ptr_addr_equal_to(x, y));
}

template <is_node_type t_node_1>
bool operator==(const node_ptr<t_node_1, false> &x, std::nullptr_t) {
  return (node_ptr_addr_equal_to(x, nullptr));
}

template <is_node_type t_node_1>
bool operator==(std::nullptr_t, const node_ptr<t_node_1, false> &x) {
  return (node_ptr_addr_equal_to(nullptr, x));
}

template <is_node_type t_node_1, is_node_type t_node_2>
bool operator!=(const node_ptr<t_node_1, false> &x,
                const node_ptr<t_node_2, false> &y) {
  return (node_ptr_addr_not_equal_to(x, y));
}

template <is_node_type t_node_1>
bool operator!=(const node_ptr<t_node_1, false> &x, std::nullptr_t) {
  return (node_ptr_addr_not_equal_to(x, nullptr));
}

template <is_node_type t_node_1>
bool operator!=(std::nullptr_t, const node_ptr<t_node_1, false> &x) {
  return (node_ptr_addr_not_equal_to(nullptr, x));
}

template <is_node_type t_node_1, is_node_type t_node_2>
bool operator>(const node_ptr<t_node_1, false> &x,
               const node_ptr<t_node_2, false> &y) {
  return (node_ptr_addr_greater(x, y));
}

template <is_node_type t_node_1>
bool operator>(const node_ptr<t_node_1, false> &x, std::nullptr_t) {
  return (node_ptr_addr_greater(x, nullptr));
}

template <is_node_type t_node_1>
bool operator>(std::nullptr_t, const node_ptr<t_node_1, false> &x) {
  return (node_ptr_addr_greater(nullptr, x));
}

template <is_node_type t_node_1, is_node_type t_node_2>
bool operator<(const node_ptr<t_node_1, false> &x,
               const node_ptr<t_node_2, false> &y) {
  return (node_ptr_addr_less(x, y));
}

template <is_node_type t_node_1>
bool operator<(const node_ptr<t_node_1, false> &x, std::nullptr_t) {
  return (node_ptr_addr_less(x, nullptr));
}

template <is_node_type t_node_1>
bool operator<(std::nullptr_t, const node_ptr<t_node_1, false> &x) {
  return (node_ptr_addr_less(nullptr, x));
}

template <is_node_type t_node_1, is_node_type t_node_2>
bool operator>=(const node_ptr<t_node_1, false> &x,
                const node_ptr<t_node_2, false> &y) {
  return (node_ptr_addr_greater_equal(x, y));
}

template <is_node_type t_node_1>
bool operator>=(const node_ptr<t_node_1, false> &x, std::nullptr_t) {
  return (node_ptr_addr_greater_equal(x, nullptr));
}

template <is_node_type t_node_1>
bool operator>=(std::nullptr_t, const node_ptr<t_node_1, false> &x) {
  return (node_ptr_addr_greater_equal(nullptr, x));
}

template <is_node_type t_node_1, is_node_type t_node_2>
bool operator<=(const node_ptr<t_node_1, false> &x,
                const node_ptr<t_node_2, false> &y) {
  return (node_ptr_addr_less_equal(x, y));
}

template <is_node_type t_node_1>
bool operator<=(const node_ptr<t_node_1, false> &x, std::nullptr_t) {
  return (node_ptr_addr_less_equal(x, nullptr));
}

template <is_node_type t_node_1>
bool operator<=(std::nullptr_t, const node_ptr<t_node_1, false> &x) {
  return (node_ptr_addr_less_equal(nullptr, x));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1,
          is_node_type t_node_2, bool t_compare_equality_by_value_2>
  requires std::three_way_comparable<node *>
std::compare_three_way_result_t<node *>
operator<=>(const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
            const node_ptr<t_node_2, t_compare_equality_by_value_2> &y) {
  return (std::compare_three_way{}((static_cast<node *>(x.get())),
                                   (static_cast<node *>(y.get()))));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
  requires std::three_way_comparable<node *>
std::compare_three_way_result_t<node *>
operator<=>(const node_ptr<t_node_1, t_compare_equality_by_value_1> &x,
            std::nullptr_t) {
  return (std::compare_three_way{}((static_cast<node *>(x.get())),
                                   (static_cast<node *>(nullptr))));
}

template <is_node_type t_node_1, bool t_compare_equality_by_value_1>
  requires std::three_way_comparable<node *>
std::compare_three_way_result_t<node *>
operator<=>(std::nullptr_t,
            const node_ptr<t_node_1, t_compare_equality_by_value_1> &x) {
  return (std::compare_three_way{}((static_cast<node *>(nullptr)),
                                   (static_cast<node *>(x.get()))));
}

template <is_node_type t_node_1, is_node_type t_node_2>
bool operator==(const node_ptr<t_node_1, true> &x,
                const node_ptr<t_node_2, true> &y) {
  return (node_ptr_val_equal_to(x, y));
}

template <is_node_type t_node_1, is_node_type t_node_2>
bool operator!=(const node_ptr<t_node_1, true> &x,
                const node_ptr<t_node_2, true> &y) {
  return (node_ptr_val_not_equal_to(x, y));
}

} // namespace libconfigfile

#endif
