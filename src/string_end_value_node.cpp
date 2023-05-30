#include "string_end_value_node.hpp"

#include "character_constants.hpp"
#include "end_value_node.hpp"
#include "node_types.hpp"

#include <iostream>
#include <string>
#include <type_traits>

libconfigfile::string_end_value_node::string_end_value_node() : base_t{} {}

libconfigfile::string_end_value_node::string_end_value_node(
    const string_end_value_node &other)
    : base_t{other} {}

libconfigfile::string_end_value_node::string_end_value_node(
    string_end_value_node
        &&other) noexcept(std::is_nothrow_move_constructible_v<base_t>)
    : base_t{std::move(other)} {}

libconfigfile::string_end_value_node::string_end_value_node(const base_t &other)
    : base_t{other} {}

libconfigfile::string_end_value_node::string_end_value_node(
    base_t &&other) noexcept(std::is_nothrow_move_constructible_v<base_t>)
    : base_t{std::move(other)} {}

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

std::ostream &
libconfigfile::string_end_value_node::print(std::ostream &out) const {
  out << character_constants::g_k_string_delimiter << *this
      << character_constants::g_k_string_delimiter;
  return out;
}

libconfigfile::string_end_value_node &
libconfigfile::string_end_value_node::operator=(
    const string_end_value_node &other) {
  base_t::operator=(other);
  return *this;
}

libconfigfile::string_end_value_node &
libconfigfile::string_end_value_node::operator=(
    string_end_value_node
        &&other) noexcept(std::is_nothrow_move_assignable_v<base_t>) {
  base_t::operator=(std::move(other));
  return *this;
}

libconfigfile::string_end_value_node &
libconfigfile::string_end_value_node::operator=(const base_t &other) {
  base_t::operator=(other);
  return *this;
}

libconfigfile::string_end_value_node &
libconfigfile::string_end_value_node::operator=(base_t &&other) noexcept(
    std::is_nothrow_move_assignable_v<base_t>) {
  base_t::operator=(std::move(other));
  return *this;
}

std::ostream &libconfigfile::operator<<(std::ostream &out,
                                        const string_end_value_node &n) {
  return n.print(out);
}
