#include "map_node.hpp"

#include "character_constants.hpp"
#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"

#include <iostream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

libconfigfile::map_node::map_node() : base_t{} {}

libconfigfile::map_node::map_node(const map_node &other) : base_t{other} {}

libconfigfile::map_node::map_node(map_node &&other) noexcept(
    std::is_nothrow_move_constructible_v<base_t>)
    : base_t{std::move(other)} {}

libconfigfile::map_node::map_node(const base_t &other) : base_t{other} {}

libconfigfile::map_node::map_node(base_t &&other) noexcept(
    std::is_nothrow_move_constructible_v<base_t>)
    : base_t{std::move(other)} {}

libconfigfile::map_node::~map_node(){};

libconfigfile::map_node *libconfigfile::map_node::create_new() const {
  return new map_node{};
}

libconfigfile::map_node *libconfigfile::map_node::create_clone() const {
  return new map_node{*this};
}

libconfigfile::node_type libconfigfile::map_node::get_node_type() const {
  return libconfigfile::node_type::Map;
}

bool libconfigfile::map_node::polymorphic_value_compare(
    const node *other) const {
  if ((other->get_node_type()) == (libconfigfile::node_type::Map)) {
    return ((*(dynamic_cast<const map_node *>(other))) == (*this));
  } else {
    return false;
  }
}

std::string libconfigfile::map_node::serialize(int indent_level /*= 0*/) const {
  std::string ret_val;

  if (m_is_root_map == false) {
    (ret_val += character_constants::k_map_opening_delimiter) +=
        character_constants::k_newline;
  }

  for (auto p{this->begin()}; p != this->end(); ++p) {
    for (int i{0}; i < indent_level; ++i) {
      ret_val += character_constants::k_indent_str;
    }

    (ret_val += (*p).first) += character_constants::k_key_value_assign;
    ret_val += (*p).second->serialize(indent_level + 1);
    (ret_val += character_constants::k_key_value_terminate) +=
        character_constants::k_newline;
  }

  if (m_is_root_map == false) {
    ret_val += character_constants::k_map_closing_delimiter;
  }

  return ret_val;
}

std::ostream &libconfigfile::map_node::print(std::ostream &out,
                                             int indent_level /*= 0*/) const {
  out << serialize(indent_level);
  return out;
}

bool libconfigfile::map_node::get_is_root_map() const { return m_is_root_map; }

void libconfigfile::map_node::set_is_root_map(const bool is_root_map) {
  m_is_root_map = is_root_map;
}

libconfigfile::map_node &
libconfigfile::map_node::operator=(const map_node &other) {
  base_t::operator=(other);
  return *this;
}

libconfigfile::map_node &libconfigfile::map_node::operator=(
    map_node &&other) noexcept(std::is_nothrow_move_assignable_v<base_t>) {
  base_t::operator=(std::move(other));
  return *this;
}

libconfigfile::map_node &
libconfigfile::map_node::operator=(const base_t &other) {
  base_t::operator=(other);
  return *this;
}

libconfigfile::map_node &libconfigfile::map_node::operator=(
    base_t &&other) noexcept(std::is_nothrow_move_assignable_v<base_t>) {
  base_t::operator=(std::move(other));
  return *this;
}

std::ostream &libconfigfile::operator<<(std::ostream &out, const map_node &n) {
  return n.print(out);
}

libconfigfile::map_node::base_t
libconfigfile::node_to_base(const map_node &node) {
  return map_node::base_t{node};
}

libconfigfile::map_node::base_t libconfigfile::node_to_base(map_node &&node) {
  return map_node::base_t{std::move(node)};
}
