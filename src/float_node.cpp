#include "float_node.hpp"

#include "character_constants.hpp"
#include "node.hpp"
#include "node_types.hpp"

#include <iostream>
#include <type_traits>
#include <utility>

libconfigfile::float_node::float_node() : m_value{} {}

libconfigfile::float_node::float_node(const base_t value) : m_value{value} {}

libconfigfile::float_node::float_node(const float_node &other)
    : m_value{other.m_value} {}

libconfigfile::float_node::float_node(float_node &&other) noexcept(
    std::is_nothrow_move_constructible_v<base_t>)
    : m_value{other.m_value} {}

libconfigfile::float_node::~float_node() {}

libconfigfile::float_node *libconfigfile::float_node::create_new() const {
  return new float_node{};
}

libconfigfile::float_node *libconfigfile::float_node::create_clone() const {
  return new float_node{*this};
}

libconfigfile::node_type libconfigfile::float_node::get_node_type() const {
  return node_type::Float;
}

bool libconfigfile::float_node::polymorphic_value_compare(
    const node *other) const {
  if ((other->get_node_type()) == (node_type::Float)) {
    return ((*(dynamic_cast<const float_node *>(other))) == (*this));
  } else {
    return false;
  }
}

std::ostream &libconfigfile::float_node::print(
    std::ostream &out, [[maybe_unused]] const int indent_level /*= 0*/) const {
  out << m_value;
  return out;
}

libconfigfile::float_node::base_t libconfigfile::float_node::get() const {
  return m_value;
}

void libconfigfile::float_node::set(const base_t value) { m_value = value; }

libconfigfile::float_node &
libconfigfile::float_node::operator=(const float_node &other) {
  m_value = other.m_value;
  return *this;
}

libconfigfile::float_node &libconfigfile::float_node::operator=(
    float_node &&other) noexcept(std::is_nothrow_move_assignable_v<base_t>) {
  m_value = other.m_value;
  return *this;
}

libconfigfile::float_node &
libconfigfile::float_node::operator=(const base_t value) {
  m_value = value;
  return *this;
}

bool libconfigfile::operator==(const float_node &x, const float_node &y) {
  return ((x.get()) == (y.get()));
}

bool libconfigfile::operator!=(const float_node &x, const float_node &y) {
  return (!(x == y));
}

std::ostream &libconfigfile::operator<<(std::ostream &out,
                                        const float_node &n) {
  return n.print(out);
}

libconfigfile::float_node::base_t
libconfigfile::node_to_base(const float_node &node) {
  return float_node::base_t{node.m_value};
}

libconfigfile::float_node::base_t
libconfigfile::node_to_base(float_node &&node) {
  return float_node::base_t{std::move(node.m_value)};
}
