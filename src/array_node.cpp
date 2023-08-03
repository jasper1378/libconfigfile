#include "array_node.hpp"

#include "character_constants.hpp"
#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"

#include <iostream>
#include <type_traits>
#include <vector>

libconfigfile::array_node::array_node() : base_t{} {}

libconfigfile::array_node::array_node(const array_node &other)
    : base_t{other} {}

libconfigfile::array_node::array_node(array_node &&other) noexcept(
    std::is_nothrow_move_constructible_v<base_t>)
    : base_t{std::move(other)} {}

libconfigfile::array_node::array_node(const base_t &other) : base_t{other} {}

libconfigfile::array_node::array_node(base_t &&other) noexcept(
    std::is_nothrow_move_constructible_v<base_t>)
    : base_t{std::move(other)} {}

libconfigfile::array_node::~array_node() {}

libconfigfile::array_node *libconfigfile::array_node::create_new() const {
  return new array_node{};
}

libconfigfile::array_node *libconfigfile::array_node::create_clone() const {
  return new array_node{*this};
}

libconfigfile::node_type libconfigfile::array_node::get_node_type() const {
  return node_type::ARRAY;
}

bool libconfigfile::array_node::polymorphic_value_compare(
    const node *other) const {
  if ((other->get_node_type()) == (node_type::ARRAY)) {
    return ((*(dynamic_cast<const array_node *>(other))) == (*this));
  } else {
    return false;
  }
}

std::ostream &libconfigfile::array_node::print(std::ostream &out) const {
  out << character_constants::g_k_array_opening_delimiter;

  for (auto p{this->begin()}; p != this->end(); ++p) {
    out << (*p);

    if ((p + 1) != this->end()) {
      out << character_constants::g_k_array_element_separator;
    }
  }

  out << character_constants::g_k_array_closing_delimiter;

  return out;
}

libconfigfile::array_node &
libconfigfile::array_node::operator=(const array_node &other) {
  base_t::operator=(other);
  return *this;
}

libconfigfile::array_node &libconfigfile::array_node::operator=(
    array_node &&other) noexcept(std::is_nothrow_move_assignable_v<base_t>) {
  base_t::operator=(std::move(other));
  return *this;
}

libconfigfile::array_node &
libconfigfile::array_node::operator=(const base_t &other) {
  base_t::operator=(other);
  return *this;
}

libconfigfile::array_node &libconfigfile::array_node::operator=(
    base_t &&other) noexcept(std::is_nothrow_move_assignable_v<base_t>) {
  base_t::operator=(std::move(other));
  return *this;
}

std::ostream &libconfigfile::operator<<(std::ostream &out,
                                        const array_node &n) {
  return n.print(out);
}
