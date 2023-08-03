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
  return libconfigfile::node_type::MAP;
}

bool libconfigfile::map_node::polymorphic_value_compare(
    const node *other) const {
  if ((other->get_node_type()) == (libconfigfile::node_type::MAP)) {
    return ((*(dynamic_cast<const map_node *>(other))) == (*this));
  } else {
    return false;
  }
}

std::ostream &libconfigfile::map_node::print(std::ostream &out) const {
  for (auto p{this->begin()}; p != this->end(); ++p) {

    switch ((*p).second->get_node_type()) {

    case libconfigfile::node_type::MAP: {
      out << character_constants::g_k_section_name_opening_delimiter
          << (*p).first
          << character_constants::g_k_section_name_closing_delimiter << '\n';
      out << character_constants::g_k_section_body_opening_delimiter << '\n';
      out << (*p).second;
      out << character_constants ::g_k_section_body_closing_delimiter << '\n';
    } break;

    default: {
      out << (*p).first << character_constants::g_k_key_value_assign
          << (*p).second << character_constants::g_k_key_value_terminate
          << '\n';
    } break;
    }
  }
  return out;
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
