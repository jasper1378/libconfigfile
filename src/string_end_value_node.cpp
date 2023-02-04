#include "string_end_value_node.hpp"

#include "end_value_node.hpp"
#include "node.hpp"
#include "node_types.hpp"
#include "value_node.hpp"

#include <cstddef>
#include <iostream>
#include <string>
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
    string_end_value_node &&other)
    : m_value{std::move(other.m_value)} {}

libconfigfile::string_end_value_node::~string_end_value_node() {}

libconfigfile::actual_node_type
libconfigfile::string_end_value_node::get_actual_node_type() const {
  return actual_node_type::STRING_END_VALUE_NODE;
}

libconfigfile::string_end_value_node *
libconfigfile::string_end_value_node::create_new() const {
  return new string_end_value_node{};
}

libconfigfile::string_end_value_node *
libconfigfile::string_end_value_node::create_clone() const {
  return new string_end_value_node{*this};
}

libconfigfile::end_value_node_type
libconfigfile::string_end_value_node::get_end_value_node_type() const {
  return end_value_node_type::STRING;
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
  if (this == &other) {
    return *this;
  }

  m_value = other.m_value;

  return *this;
}

libconfigfile::string_end_value_node &
libconfigfile::string_end_value_node::operator=(string_end_value_node &&other) {
  if (this == &other) {
    return *this;
  }

  m_value = other.m_value;

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

libconfigfile::string_end_value_node::operator value_t() const {
  return m_value;
}

std::ostream &libconfigfile::operator<<(std::ostream &out,
                                        const string_end_value_node &s) {
  out << s.m_value;
  return out;
}

std::istream &libconfigfile::operator>>(std::istream &in,
                                        string_end_value_node &s) {
  in >> s.m_value;
  return in;
}
