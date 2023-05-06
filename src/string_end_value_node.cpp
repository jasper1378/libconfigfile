#include "string_end_value_node.hpp"

#include "end_value_node.hpp"
#include "node_types.hpp"

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

libconfigfile::string_end_value_node::string_end_value_node() : m_value{} {}

libconfigfile::string_end_value_node::string_end_value_node(
    const value_t &value)
    : m_value{value} {}

libconfigfile::string_end_value_node::string_end_value_node(value_t &&value)
    : m_value{std::move(value)} {}

libconfigfile::string_end_value_node::string_end_value_node(
    const string_end_value_node &other)
    : m_value{other.m_value} {}

libconfigfile::string_end_value_node::string_end_value_node(
    string_end_value_node
        &&other) noexcept(std::is_nothrow_move_constructible_v<value_t>)
    : m_value{std::move(other.m_value)} {}

libconfigfile::string_end_value_node::~string_end_value_node() {}

libconfigfile::string_end_value_node *
libconfigfile::string_end_value_node::create_new() const {
  return new string_end_value_node{};
}

libconfigfile::string_end_value_node *
libconfigfile::string_end_value_node::create_clone() const {
  return new string_end_value_node{*this};
}

libconfigfile::absolute_node_type
libconfigfile::string_end_value_node::get_absolute_node_type() const {
  return absolute_node_type::STRING;
}

libconfigfile::end_value_node_type
libconfigfile::string_end_value_node::get_end_value_node_type() const {
  return end_value_node_type::INTEGER;
}

bool libconfigfile::string_end_value_node::polymorphic_value_compare(
    const node *other) const {
  if ((other->get_absolute_node_type()) == (absolute_node_type::STRING)) {
    return ((*(dynamic_cast<const string_end_value_node *>(other))) == (*this));
  } else {
    return false;
  }
}

void libconfigfile::string_end_value_node::print(std::ostream &out) const {
  out << m_value;
}

const libconfigfile::string_end_value_node::value_t &
libconfigfile::string_end_value_node::get() const {
  return m_value;
}

libconfigfile::string_end_value_node::value_t &
libconfigfile::string_end_value_node::get() {
  return m_value;
}

void libconfigfile::string_end_value_node::set(const value_t &value) {
  m_value = value;
}

void libconfigfile::string_end_value_node::set(value_t &&value) {
  m_value = std::move(value);
}

libconfigfile::string_end_value_node &
libconfigfile::string_end_value_node::operator=(
    const string_end_value_node &other) {
  m_value = other.m_value;
  return *this;
}

libconfigfile::string_end_value_node &
libconfigfile::string_end_value_node::operator=(
    string_end_value_node
        &&other) noexcept(std::is_nothrow_move_assignable_v<value_t>) {
  m_value = std::move(other.m_value);
  return *this;
}

libconfigfile::string_end_value_node &
libconfigfile::string_end_value_node::operator=(const value_t &value) {
  m_value = value;
  return *this;
}

libconfigfile::string_end_value_node &
libconfigfile::string_end_value_node::operator=(value_t &&value) {
  m_value = std::move(value);
  return *this;
}

bool libconfigfile::operator==(const string_end_value_node &x,
                               const string_end_value_node &y) {
  return ((x.get()) == (y.get()));
}

bool libconfigfile::operator!=(const string_end_value_node &x,
                               const string_end_value_node &y) {
  return ((x.get()) == (y.get()));
}

std::ostream &libconfigfile::operator<<(std::ostream &out,
                                        const string_end_value_node &n) {
  n.print(out);
  return out;
}
