#include "integer_end_value_node.hpp"

#include "end_value_node.hpp"
#include "node.hpp"
#include "node_types.hpp"
#include "value_node.hpp"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <utility>

libconfigfile::integer_end_value_node::integer_end_value_node() : m_value{} {}

libconfigfile::integer_end_value_node::integer_end_value_node(
    const value_t value)
    : m_value{value} {}

libconfigfile::integer_end_value_node::integer_end_value_node(
    const integer_end_value_node &other)
    : m_value{other.m_value} {}

libconfigfile::integer_end_value_node::integer_end_value_node(
    integer_end_value_node &&other) noexcept
    : m_value{std::move(other.m_value)} {}

libconfigfile::integer_end_value_node::~integer_end_value_node() {}

libconfigfile::actual_node_type
libconfigfile::integer_end_value_node::get_actual_node_type() const {
  return actual_node_type::INTEGER_END_VALUE_NODE;
}

libconfigfile::integer_end_value_node *
libconfigfile::integer_end_value_node::create_new() const {
  return new integer_end_value_node{};
}

libconfigfile::integer_end_value_node *
libconfigfile::integer_end_value_node::create_clone() const {
  return new integer_end_value_node{*this};
}

libconfigfile::end_value_node_type
libconfigfile::integer_end_value_node::get_end_value_node_type() const {
  return end_value_node_type::INTEGER;
}

libconfigfile::integer_end_value_node::value_t
libconfigfile::integer_end_value_node::get() const {
  return m_value;
}

libconfigfile::integer_end_value_node::value_t &
libconfigfile::integer_end_value_node::get() {
  return m_value;
}

void libconfigfile::integer_end_value_node::set(const value_t value) {
  m_value = value;
}

libconfigfile::integer_end_value_node &
libconfigfile::integer_end_value_node::operator=(
    const integer_end_value_node &other) {
  if (this == &other) {
    return *this;
  }

  m_value = other.m_value;

  return *this;
}

libconfigfile::integer_end_value_node &
libconfigfile::integer_end_value_node::operator=(
    integer_end_value_node &&other) noexcept {
  if (this == &other) {
    return *this;
  }

  m_value = other.m_value;

  return *this;
}

libconfigfile::integer_end_value_node &
libconfigfile::integer_end_value_node::operator=(const value_t value) {
  m_value = value;

  return *this;
}

libconfigfile::integer_end_value_node::operator value_t() const {
  return m_value;
}

std::ostream &libconfigfile::operator<<(std::ostream &out,
                                        const integer_end_value_node &i) {
  out << i.m_value;
  return out;
}

std::istream &libconfigfile::operator>>(std::istream &in,
                                        integer_end_value_node &i) {
  in >> i.m_value;
  return in;
}
