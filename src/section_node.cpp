#include "section_node.hpp"

#include "character_constants.hpp"
#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"

#include <iostream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

libconfigfile::section_node::section_node() : base_t{} {}

libconfigfile::section_node::section_node(const section_node &other)
    : base_t{other} {}

libconfigfile::section_node::section_node(section_node &&other) noexcept(
    std::is_nothrow_move_constructible_v<base_t>)
    : base_t{std::move(other)} {}

libconfigfile::section_node::section_node(const base_t &other)
    : base_t{other} {}

libconfigfile::section_node::section_node(base_t &&other) noexcept(
    std::is_nothrow_move_constructible_v<base_t>)
    : base_t{std::move(other)} {}

libconfigfile::section_node::~section_node(){};

libconfigfile::section_node *libconfigfile::section_node::create_new() const {
  return new section_node{};
}

libconfigfile::section_node *libconfigfile::section_node::create_clone() const {
  return new section_node{*this};
}

libconfigfile::absolute_node_type
libconfigfile::section_node::get_absolute_node_type() const {
  return absolute_node_type::SECTION;
}

libconfigfile::node_type libconfigfile::section_node::get_node_type() const {
  return libconfigfile::node_type::SECTION;
}

bool libconfigfile::section_node::polymorphic_value_compare(
    const node *other) const {
  if ((other->get_absolute_node_type()) == (absolute_node_type::SECTION)) {
    return ((*(dynamic_cast<const section_node *>(other))) == (*this));
  } else {
    return false;
  }
}

void libconfigfile::section_node::print(std::ostream &out) const {
  for (auto p{this->begin()}; p != this->end(); ++p) {

    switch ((*p).second->get_absolute_node_type()) {

    case absolute_node_type::SECTION: {
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
}

libconfigfile::section_node &
libconfigfile::section_node::operator=(const section_node &other) {
  base_t::operator=(other);
  return *this;
}

libconfigfile::section_node &libconfigfile::section_node::operator=(
    section_node &&other) noexcept(std::is_nothrow_move_assignable_v<base_t>) {
  base_t::operator=(std::move(other));
  return *this;
}

libconfigfile::section_node &
libconfigfile::section_node::operator=(const base_t &other) {
  base_t::operator=(other);
  return *this;
}

libconfigfile::section_node &libconfigfile::section_node::operator=(
    base_t &&other) noexcept(std::is_nothrow_move_assignable_v<base_t>) {
  base_t::operator=(std::move(other));
  return *this;
}

std::ostream &libconfigfile::operator<<(std::ostream &out,
                                        const section_node &n) {
  n.print(out);
  return out;
}
