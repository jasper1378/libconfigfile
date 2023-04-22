#include "integer_end_value_node.hpp"

#include "end_value_node.hpp"
#include "node_types.hpp"

#include <cstdint>
#include <type_traits>
#include <utility>

libconfigfile::integer_end_value_node::integer_end_value_node() : m_value{} {}

libconfigfile::integer_end_value_node::integer_end_value_node(
    const value_t &value)
    : m_value{value} {}

libconfigfile::integer_end_value_node::integer_end_value_node(value_t &&value)
    : m_value{std::move(value)} {}

libconfigfile::integer_end_value_node::integer_end_value_node(
    const integer_end_value_node &other)
    : m_value{other.m_value} {}

libconfigfile::integer_end_value_node::integer_end_value_node(
    integer_end_value_node
        &&other) noexcept(std::is_nothrow_move_constructible_v<value_t>)
    : m_value{std::move(other.m_value)} {}

libconfigfile::integer_end_value_node::~integer_end_value_node() {}

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

const libconfigfile::integer_end_value_node::value_t &
libconfigfile::integer_end_value_node::get() const {
  return m_value;
}

libconfigfile::integer_end_value_node::value_t &
libconfigfile::integer_end_value_node::get() {
  return m_value;
}

void libconfigfile::integer_end_value_node::set(const value_t &value) {
  m_value = value;
}

void libconfigfile::integer_end_value_node::set(value_t &&value) {
  m_value = std::move(value);
}

libconfigfile::integer_end_value_node &
libconfigfile::integer_end_value_node::operator=(
    const integer_end_value_node &other) {
  m_value = other.m_value;
  return *this;
}

libconfigfile::integer_end_value_node &
libconfigfile::integer_end_value_node::operator=(
    integer_end_value_node
        &&other) noexcept(std::is_nothrow_move_assignable_v<value_t>) {
  m_value = std::move(other.m_value);
  return *this;
}

libconfigfile::integer_end_value_node &
libconfigfile::integer_end_value_node::operator=(const value_t &value) {
  m_value = value;
  return *this;
}

libconfigfile::integer_end_value_node &
libconfigfile::integer_end_value_node::operator=(value_t &&value) {
  m_value = std::move(value);
  return *this;
}