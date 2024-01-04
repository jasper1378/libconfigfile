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
    const base_t value,
    const numeral_system *num_sys /*= &character_constants::g_k_dec_num_sys*/)
    : m_value{value}, m_num_sys{num_sys} {}

libconfigfile::integer_node::integer_node(const integer_node &other)
    : m_value{other.m_value}, m_num_sys{other.m_num_sys} {}

libconfigfile::integer_node::integer_node(integer_node &&other) noexcept(
    std::is_nothrow_move_constructible_v<base_t>)
    : m_value{other.m_value}, m_num_sys{other.m_num_sys} {}

libconfigfile::integer_node::~integer_node() {}

libconfigfile::integer_node *libconfigfile::integer_node::create_new() const {
  return new integer_node{};
}

libconfigfile::integer_node *libconfigfile::integer_node::create_clone() const {
  return new integer_node{*this};
}

libconfigfile::node_type libconfigfile::integer_node::get_node_type() const {
  return node_type::Integer;
}

bool libconfigfile::integer_node::polymorphic_value_compare(
    const node *other) const {
  if ((other->get_node_type()) == (node_type::Integer)) {
    return ((*(dynamic_cast<const integer_node *>(other))) == (*this));
  } else {
    return false;
  }
}

std::string libconfigfile::integer_node::serialize(
    [[maybe_unused]] int indent_level /*= 0*/) const {
  std::string ret_val;

  switch (m_num_sys->base) {
  case numeral_system_decimal.base: {
    ret_val += std::format("{:d}", m_value);
  } break;

  case numeral_system_binary.base: {
    ret_val += character_constants::g_k_num_sys_prefix_leader;
    ret_val += m_num_sys->prefix;
    ret_val += std::format("{:b}", m_value);
  } break;

  case numeral_system_octal.base: {
    ret_val += character_constants::g_k_num_sys_prefix_leader;
    ret_val += m_num_sys->prefix;
    ret_val += std::format("{:o}", m_value);
  } break;

  case numeral_system_hexadecimal.base: {
    ret_val += character_constants::g_k_num_sys_prefix_leader;
    ret_val += m_num_sys->prefix;
    ret_val += std::format("{:x}", m_value);
  } break;
  }

  return ret_val;
}

std::ostream &libconfigfile::integer_node::print(
    std::ostream &out, [[maybe_unused]] const int indent_level /*= 0*/) const {
  out << serialize();
  return out;
}

libconfigfile::integer_node::base_t libconfigfile::integer_node::get() const {
  return m_value;
}

void libconfigfile::integer_node::set(const base_t value) { m_value = value; }

const libconfigfile::numeral_system *
libconfigfile::integer_node::get_num_sys() const {
  return m_num_sys;
}

void libconfigfile::integer_node::set_num_sys(const numeral_system *num_sys) {
  m_num_sys = num_sys;
}

std::pair<libconfigfile::integer_node::base_t,
          const libconfigfile::numeral_system *>
libconfigfile::integer_node::get_both() const {
  return {m_value, m_num_sys};
}

void libconfigfile::integer_node::set_both(
    const std::pair<base_t, const libconfigfile::numeral_system *> &both) {
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
    integer_node &&other) noexcept(std::is_nothrow_move_assignable_v<base_t>) {
  m_value = other.m_value;
  m_num_sys = other.m_num_sys;
  return *this;
}

libconfigfile::integer_node &
libconfigfile::integer_node::operator=(const base_t value) {
  m_value = value;
  return *this;
}

libconfigfile::integer_node &libconfigfile::integer_node::operator=(
    const std::pair<base_t, const libconfigfile::numeral_system *> &both) {
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

libconfigfile::integer_node::base_t
libconfigfile::node_to_base(const integer_node &node) {
  return integer_node::base_t{node.m_value};
}

libconfigfile::integer_node::base_t
libconfigfile::node_to_base(integer_node &&node) {
  return integer_node::base_t{std::move(node.m_value)};
}
