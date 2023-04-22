#include "float_end_value_node.hpp"

#include "end_value_node.hpp"
#include "node_types.hpp"

#include <type_traits>
#include <utility>

libconfigfile::float_end_value_node::float_end_value_node() : m_value{} {}

libconfigfile::float_end_value_node::float_end_value_node(const value_t &value)
    : m_value{value} {}

libconfigfile::float_end_value_node::float_end_value_node(value_t &&value)
    : m_value{std::move(value)} {}

libconfigfile::float_end_value_node::float_end_value_node(
    const float_end_value_node &other)
    : m_value{other.m_value} {}

libconfigfile::float_end_value_node::float_end_value_node(
    float_end_value_node
        &&other) noexcept(std::is_nothrow_move_constructible_v<value_t>)
    : m_value{std::move(other.m_value)} {}

libconfigfile::float_end_value_node::~float_end_value_node() {}

libconfigfile::float_end_value_node *
libconfigfile::float_end_value_node::create_new() const {
  return new float_end_value_node{};
}

libconfigfile::float_end_value_node *
libconfigfile::float_end_value_node::create_clone() const {
  return new float_end_value_node{*this};
}

libconfigfile::end_value_node_type
libconfigfile::float_end_value_node::get_end_value_node_type() const {
  return end_value_node_type::INTEGER;
}

const libconfigfile::float_end_value_node::value_t &
libconfigfile::float_end_value_node::get() const {
  return m_value;
}

libconfigfile::float_end_value_node::value_t &
libconfigfile::float_end_value_node::get() {
  return m_value;
}

void libconfigfile::float_end_value_node::set(const value_t &value) {
  m_value = value;
}

void libconfigfile::float_end_value_node::set(value_t &&value) {
  m_value = std::move(value);
}

libconfigfile::float_end_value_node &
libconfigfile::float_end_value_node::operator=(
    const float_end_value_node &other) {
  m_value = other.m_value;
  return *this;
}

libconfigfile::float_end_value_node &
libconfigfile::float_end_value_node::operator=(
    float_end_value_node
        &&other) noexcept(std::is_nothrow_move_assignable_v<value_t>) {
  m_value = std::move(other.m_value);
  return *this;
}

libconfigfile::float_end_value_node &
libconfigfile::float_end_value_node::operator=(const value_t &value) {
  m_value = value;
  return *this;
}

libconfigfile::float_end_value_node &
libconfigfile::float_end_value_node::operator=(value_t &&value) {
  m_value = std::move(value);
  return *this;
}
