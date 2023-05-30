#include "float_end_value_node.hpp"

#include "end_value_node.hpp"
#include "node_types.hpp"

#include <iostream>
#include <type_traits>
#include <utility>

libconfigfile::float_end_value_node::float_end_value_node() : m_value{} {}

libconfigfile::float_end_value_node::float_end_value_node(value_t value)
    : m_value{value} {}

libconfigfile::float_end_value_node::float_end_value_node(
    const float_end_value_node &other)
    : m_value{other.m_value} {}

libconfigfile::float_end_value_node::float_end_value_node(
    float_end_value_node
        &&other) noexcept(std::is_nothrow_move_constructible_v<value_t>)
    : m_value{other.m_value} {}

libconfigfile::float_end_value_node::~float_end_value_node() {}

libconfigfile::float_end_value_node *
libconfigfile::float_end_value_node::create_new() const {
  return new float_end_value_node{};
}

libconfigfile::float_end_value_node *
libconfigfile::float_end_value_node::create_clone() const {
  return new float_end_value_node{*this};
}

libconfigfile::absolute_node_type
libconfigfile::float_end_value_node::get_absolute_node_type() const {
  return absolute_node_type::FLOAT;
}

libconfigfile::end_value_node_type
libconfigfile::float_end_value_node::get_end_value_node_type() const {
  return end_value_node_type::INTEGER;
}

bool libconfigfile::float_end_value_node::polymorphic_value_compare(
    const node *other) const {
  if ((other->get_absolute_node_type()) == (absolute_node_type::FLOAT)) {
    return ((*(dynamic_cast<const float_end_value_node *>(other))) == (*this));
  } else {
    return false;
  }
}

std::ostream &
libconfigfile::float_end_value_node::print(std::ostream &out) const {
  out << m_value;
  return out;
}

libconfigfile::float_end_value_node::value_t
libconfigfile::float_end_value_node::get() const {
  return m_value;
}

void libconfigfile::float_end_value_node::set(value_t value) {
  m_value = value;
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
  m_value = other.m_value;
  return *this;
}

libconfigfile::float_end_value_node &
libconfigfile::float_end_value_node::operator=(value_t value) {
  m_value = value;
  return *this;
}

bool libconfigfile::operator==(const float_end_value_node &x,
                               const float_end_value_node &y) {
  return ((x.get()) == (y.get()));
}

bool libconfigfile::operator!=(const float_end_value_node &x,
                               const float_end_value_node &y) {
  return (!(x == y));
}

std::ostream &libconfigfile::operator<<(std::ostream &out,
                                        const float_end_value_node &n) {
  return n.print(out);
}
