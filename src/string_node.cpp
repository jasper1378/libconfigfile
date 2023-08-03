#include "string_node.hpp"

#include "character_constants.hpp"
#include "node.hpp"
#include "node_types.hpp"

#include <iostream>
#include <string>
#include <type_traits>

libconfigfile::string_node::string_node() : base_t{} {}

libconfigfile::string_node::string_node(const string_node &other)
    : base_t{other} {}

libconfigfile::string_node::string_node(string_node &&other) noexcept(
    std::is_nothrow_move_constructible_v<base_t>)
    : base_t{std::move(other)} {}

libconfigfile::string_node::string_node(const base_t &other) : base_t{other} {}

libconfigfile::string_node::string_node(base_t &&other) noexcept(
    std::is_nothrow_move_constructible_v<base_t>)
    : base_t{std::move(other)} {}

libconfigfile::string_node::~string_node() {}

libconfigfile::string_node *libconfigfile::string_node::create_new() const {
  return new string_node{};
}

libconfigfile::string_node *libconfigfile::string_node::create_clone() const {
  return new string_node{*this};
}

libconfigfile::node_type libconfigfile::string_node::get_node_type() const {
  return node_type::STRING;
}

bool libconfigfile::string_node::polymorphic_value_compare(
    const node *other) const {
  if ((other->get_node_type()) == (node_type::STRING)) {
    return ((*(dynamic_cast<const string_node *>(other))) == (*this));
  } else {
    return false;
  }
}

std::ostream &libconfigfile::string_node::print(std::ostream &out) const {
  static constexpr std::string escaped_string_delimiter{
      character_constants::g_k_escape_leader +
      character_constants::g_k_string_delimiter};
  out << character_constants::g_k_string_delimiter;

  std::string::size_type pos{0};
  std::string::size_type pos_prev{0};
  while (true) {
    pos = this->find(character_constants::g_k_string_delimiter, pos_prev);
    if (pos == std::string::npos) {
      break;
    } else {
      out << this->substr(pos_prev, (pos - pos_prev));
      out << escaped_string_delimiter;
      pos_prev = pos + 1;
    }
  }
  out << this->substr(pos_prev);

  out << character_constants::g_k_string_delimiter;

  return out;
}

libconfigfile::string_node &
libconfigfile::string_node::operator=(const string_node &other) {
  base_t::operator=(other);
  return *this;
}

libconfigfile::string_node &libconfigfile::string_node::operator=(
    string_node &&other) noexcept(std::is_nothrow_move_assignable_v<base_t>) {
  base_t::operator=(std::move(other));
  return *this;
}

libconfigfile::string_node &
libconfigfile::string_node::operator=(const base_t &other) {
  base_t::operator=(other);
  return *this;
}

libconfigfile::string_node &libconfigfile::string_node::operator=(
    base_t &&other) noexcept(std::is_nothrow_move_assignable_v<base_t>) {
  base_t::operator=(std::move(other));
  return *this;
}

std::ostream &libconfigfile::operator<<(std::ostream &out,
                                        const string_node &n) {
  return n.print(out);
}
