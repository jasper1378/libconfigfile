#include "integer_node.hpp"

#include "character_constants.hpp"
#include "node.hpp"
#include "node_types.hpp"
#include "numeral_system.hpp"

#include <cstdint>
#include <format>
#include <iostream>
#include <type_traits>
#include <utility>

libconfigfile::integer_node::integer_node()
    : m_value{}, m_num_sys{&numeral_system_decimal} {}

libconfigfile::integer_node::integer_node(
    const value_t value,
    const numeral_system *num_sys /*= &character_constants::g_k_dec_num_sys*/)
    : m_value{value}, m_num_sys{num_sys} {}

libconfigfile::integer_node::integer_node(const integer_node &other)
    : m_value{other.m_value}, m_num_sys{other.m_num_sys} {}

libconfigfile::integer_node::integer_node(integer_node &&other) noexcept(
    std::is_nothrow_move_constructible_v<value_t>)
    : m_value{other.m_value}, m_num_sys{other.m_num_sys} {}

libconfigfile::integer_node::~integer_node() {}

libconfigfile::integer_node *libconfigfile::integer_node::create_new() const {
  return new integer_node{};
}

libconfigfile::integer_node *libconfigfile::integer_node::create_clone() const {
  return new integer_node{*this};
}

libconfigfile::node_type libconfigfile::integer_node::get_node_type() const {
  return node_type::INTEGER;
}

bool libconfigfile::integer_node::polymorphic_value_compare(
    const node *other) const {
  if ((other->get_node_type()) == (node_type::INTEGER)) {
    return ((*(dynamic_cast<const integer_node *>(other))) == (*this));
  } else {
    return false;
  }
}

std::ostream &libconfigfile::integer_node::print(
    std::ostream &out, [[maybe_unused]] const int indent_level /*= 0*/) const {
  const std::ostreambuf_iterator<char> out_iter{out};

  switch (m_num_sys->base) {
  case numeral_system_decimal.base: {
    std::format_to(out_iter, "{:d}", m_value);
  } break;

  case numeral_system_binary.base: {
    out << character_constants::g_k_num_sys_prefix_leader;
    out << m_num_sys->prefix;
    std::format_to(out_iter, "{:b}", m_value);
  } break;

  case numeral_system_octal.base: {
    out << character_constants::g_k_num_sys_prefix_leader;
    out << m_num_sys->prefix;
    std::format_to(out_iter, "{:o}", m_value);
  } break;

  case numeral_system_hexadecimal.base: {
    out << character_constants::g_k_num_sys_prefix_leader;
    out << m_num_sys->prefix;
    std::format_to(out_iter, "{:x}", m_value);
  } break;
  }

  return out;
}

libconfigfile::integer_node::value_t libconfigfile::integer_node::get() const {
  return m_value;
}

void libconfigfile::integer_node::set(const value_t value) { m_value = value; }

const libconfigfile::numeral_system *
libconfigfile::integer_node::get_num_sys() const {
  return m_num_sys;
}

void libconfigfile::integer_node::set_num_sys(const numeral_system *num_sys) {
  m_num_sys = num_sys;
}

std::pair<libconfigfile::integer_node::value_t,
          const libconfigfile::numeral_system *>
libconfigfile::integer_node::get_both() const {
  return {m_value, m_num_sys};
}

void libconfigfile::integer_node::set_both(
    const std::pair<value_t, const libconfigfile::numeral_system *> &both) {
  m_value = both.first;
  m_num_sys = both.second;
}

libconfigfile::integer_node &
libconfigfile::integer_node::operator=(const integer_node &other) {
  m_value = other.m_value;
  m_num_sys = other.m_num_sys;
  return *this;
}

libconfigfile::integer_node &libconfigfile::integer_node::operator=(
    integer_node &&other) noexcept(std::is_nothrow_move_assignable_v<value_t>) {
  m_value = other.m_value;
  m_num_sys = other.m_num_sys;
  return *this;
}

libconfigfile::integer_node &
libconfigfile::integer_node::operator=(const value_t value) {
  m_value = value;
  return *this;
}

libconfigfile::integer_node &libconfigfile::integer_node::operator=(
    const std::pair<value_t, const libconfigfile::numeral_system *> &both) {
  m_value = both.first;
  m_num_sys = both.second;
  return *this;
}

bool libconfigfile::operator==(const integer_node &x, const integer_node &y) {
  return (((x.get()) == (y.get())) &&
          ((*(x.get_num_sys())) == (*(y.get_num_sys()))));
}

bool libconfigfile::operator!=(const integer_node &x, const integer_node &y) {
  return (!(x == y));
}

std::ostream &libconfigfile::operator<<(std::ostream &out,
                                        const integer_node &n) {
  return n.print(out);
}
