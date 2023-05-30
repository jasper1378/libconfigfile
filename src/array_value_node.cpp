#include "array_value_node.hpp"

#include "character_constants.hpp"
#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"
#include "value_node.hpp"

#include <iostream>
#include <type_traits>
#include <vector>

libconfigfile::array_value_node::array_value_node() : base_t{} {}

libconfigfile::array_value_node::array_value_node(const array_value_node &other)
    : base_t{other} {}

libconfigfile::array_value_node::array_value_node(
    array_value_node
        &&other) noexcept(std::is_nothrow_move_constructible_v<base_t>)
    : base_t{std::move(other)} {}

libconfigfile::array_value_node::array_value_node(const base_t &other)
    : base_t{other} {}

libconfigfile::array_value_node::array_value_node(base_t &&other) noexcept(
    std::is_nothrow_move_constructible_v<base_t>)
    : base_t{std::move(other)} {}

libconfigfile::array_value_node::~array_value_node() {}

libconfigfile::array_value_node *
libconfigfile::array_value_node::create_new() const {
  return new array_value_node{};
}

libconfigfile::array_value_node *
libconfigfile::array_value_node::create_clone() const {
  return new array_value_node{*this};
}

libconfigfile::absolute_node_type
libconfigfile::array_value_node::get_absolute_node_type() const {
  return absolute_node_type::ARRAY;
}

libconfigfile::value_node_type
libconfigfile::array_value_node::get_value_node_type() const {
  return value_node_type::ARRAY;
}

bool libconfigfile::array_value_node::polymorphic_value_compare(
    const node *other) const {
  if ((other->get_absolute_node_type()) == (absolute_node_type::ARRAY)) {
    return ((*(dynamic_cast<const array_value_node *>(other))) == (*this));
  } else {
    return false;
  }
}

std::ostream &libconfigfile::array_value_node::print(std::ostream &out) const {
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

libconfigfile::array_value_node &
libconfigfile::array_value_node::operator=(const array_value_node &other) {
  base_t::operator=(other);
  return *this;
}

libconfigfile::array_value_node &
libconfigfile::array_value_node::operator=(array_value_node &&other) noexcept(
    std::is_nothrow_move_assignable_v<base_t>) {
  base_t::operator=(std::move(other));
  return *this;
}

libconfigfile::array_value_node &
libconfigfile::array_value_node::operator=(const base_t &other) {
  base_t::operator=(other);
  return *this;
}

libconfigfile::array_value_node &libconfigfile::array_value_node::operator=(
    base_t &&other) noexcept(std::is_nothrow_move_assignable_v<base_t>) {
  base_t::operator=(std::move(other));
  return *this;
}

std::ostream &libconfigfile::operator<<(std::ostream &out,
                                        const array_value_node &n) {
  return n.print(out);
}
