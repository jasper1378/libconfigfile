#ifndef LIBCONFIGFILE_NODE_PTR_HPP
#define LIBCONFIGFILE_NODE_PTR_HPP

#include "node.hpp"

#include <concepts>
#include <utility>

namespace libconfigfile {
template <typename node_t>
concept is_node = std::derived_from<node_t, node>;

template <is_node node_t> class node_ptr {
  template <is_node node_other_t> friend class node_ptr;

public:
  using ptr_t = node_t *;

private:
  ptr_t m_ptr;

public:
  node_ptr() : m_ptr{nullptr} {}

  node_ptr(std::nullptr_t) : m_ptr{nullptr} {}

  explicit node_ptr(ptr_t ptr) : m_ptr{ptr} {}

  node_ptr(const node_ptr &other) : m_ptr{other.m_ptr->create_clone()} {}

  node_ptr(node_ptr &&other) : m_ptr{other.m_ptr} { other.m_ptr = nullptr; }

  ~node_ptr() {
    if (m_ptr != nullptr) {
      delete m_ptr;
      m_ptr = nullptr;
    }
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

    if (old_ptr != nullptr) {
      delete m_ptr;
    }
  }

  void swap(node_ptr &other) {
    using std::swap;
    swap(m_ptr, other.m_ptr);
  }

  ptr_t get() const { return m_ptr; }

  template <is_node cast_t> node_ptr<cast_t> cast() const {
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

  node_ptr &operator=(node_ptr &&other) {
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
  friend bool operator==(const node_ptr &first, const node_ptr &second) {
    return first.m_ptr == second.m_ptr;
  }

  friend bool operator==(const node_ptr &first, std::nullptr_t) {
    return first.m_ptr == nullptr;
  }

  friend bool operator==(std::nullptr_t, const node_ptr &second) {
    return nullptr == second.m_ptr;
  }

  friend bool operator!=(const node_ptr &first, const node_ptr &second) {
    return first.m_ptr != second.m_ptr;
  }

  friend bool operator!=(const node_ptr &first, std::nullptr_t) {
    return first.m_ptr != nullptr;
  }

  friend bool operator!=(std::nullptr_t, const node_ptr &second) {
    return nullptr != second.m_ptr;
  }
};

template <is_node node_t, typename... args_t>
node_ptr<node_t> make_node_ptr(args_t &&...args) {
  return node_ptr<node_t>{new node_t{std::forward<args_t>(args)...}};
}
} // namespace libconfigfile

#endif
