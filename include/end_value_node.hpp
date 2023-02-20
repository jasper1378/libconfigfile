#ifndef LIBCONFIGFILE_END_VALUE_NODE_HPP
#define LIBCONFIGFILE_END_VALUE_NODE_HPP

#include "node.hpp"
#include "node_types.hpp"
#include "value_node.hpp"

#include <concepts>
#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>

namespace libconfigfile {

using integer_end_value_node_t = int64_t;
using float_end_value_node_t = double;
using string_end_value_node_t = std::string;

template <typename value_t>
concept is_end_value_type = std::same_as<value_t, integer_end_value_node_t> ||
                            std::same_as<value_t, float_end_value_node_t> ||
                            std::same_as<value_t, string_end_value_node_t>;

template <is_end_value_type value_t> class end_value_node : public value_node {
private:
  value_t m_value;

public:
  end_value_node() : m_value{} {}
  explicit end_value_node(const value_t &value) : m_value{value} {}
  explicit end_value_node(value_t &&value) : m_value{std::move(value)} {}
  end_value_node(const end_value_node &other) : m_value{other.m_value} {}
  end_value_node(end_value_node &&other) noexcept(
      std::is_nothrow_move_constructible_v<value_t>)
      : m_value{std::move(other.m_value)} {}

  virtual ~end_value_node() override {}

public:
  virtual end_value_node *create_new() const override {
    return new end_value_node<value_t>{};
  }

  virtual end_value_node *create_clone() const override {
    return new end_value_node<value_t>{*this};
  }

  virtual value_node_type get_value_node_type() const override final {
    return value_node_type::END_VALUE;
  }

  virtual end_value_node_type get_end_value_node_type() const {
    return end_value_node_type::MAX;
  }

  const value_t &get() const { return m_value; }

  value_t &get() { return m_value; }

  void set(const value_t &value) { m_value = value; }

  void set(value_t &&value) { m_value = std::move(value); }

  end_value_node &operator=(const end_value_node &other) {
    m_value = other.m_value;

    return *this;
  }

  end_value_node &operator=(end_value_node &&other) noexcept(
      std::is_nothrow_move_assignable_v<value_t>) {
    m_value = std::move(other.m_value);

    return *this;
  }

  end_value_node &operator=(const value_t &value) { m_value = value; }

  end_value_node &operator=(value_t &&value) { m_value = std::move(value); }
};

template <>
inline end_value_node_type
end_value_node<integer_end_value_node_t>::get_end_value_node_type() const {
  return end_value_node_type::INTEGER;
}

template <>
inline end_value_node_type
end_value_node<float_end_value_node_t>::get_end_value_node_type() const {
  return end_value_node_type::FLOAT;
}

template <>
inline end_value_node_type
end_value_node<string_end_value_node_t>::get_end_value_node_type() const {
  return end_value_node_type::STRING;
}

} // namespace libconfigfile

#endif
