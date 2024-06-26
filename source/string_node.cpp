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
  return node_type::String;
}

bool libconfigfile::string_node::polymorphic_value_compare(
    const node *other) const {
  if ((other->get_node_type()) == (node_type::String)) {
    return ((*(dynamic_cast<const string_node *>(other))) == (*this));
  } else {
    return false;
  }
}

std::string libconfigfile::string_node::serialize(
    [[maybe_unused]] int indent_level /*=0*/) const {
  static const std::string k_need_to_replace{
      character_constants::k_control_chars +
      character_constants::k_string_delimiter +
      character_constants::k_escape_leader};

  std::string ret_val;
  ret_val.reserve(this->size() + 2);

  ret_val += character_constants::k_string_delimiter;

  std::string::size_type pos{0};
  std::string::size_type pos_prev{0};
  while (true) {
    pos = this->find_first_of(k_need_to_replace, pos_prev);
    if (pos == std::string::npos) {
      break;
    } else {
      ret_val += this->substr(pos_prev, (pos - pos_prev));
      ret_val += character_constants::k_escape_leader;

      switch (this->operator[](pos)) {
      case character_constants::k_string_delimiter: {
        ret_val += character_constants::k_string_delimiter;
      } break;
      case character_constants::k_escape_leader: {
        ret_val += character_constants::k_escape_leader;
      } break;
      default: {
        ret_val += character_constants::k_control_chars_codes.at(
            this->operator[](pos));
      } break;
      }

      pos_prev = pos + 1;
    }
  }
  ret_val += this->substr(pos_prev);

  ret_val += character_constants::k_string_delimiter;

  return ret_val;
}

std::ostream &libconfigfile::string_node::print(
    std::ostream &out, [[maybe_unused]] const int indent_level /*= 0*/) const {
  out << serialize();
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

libconfigfile::string_node::base_t
libconfigfile::node_to_base(const string_node &node) {
  return string_node::base_t{node};
}

libconfigfile::string_node::base_t
libconfigfile::node_to_base(string_node &&node) {
  return string_node::base_t{std::move(node)};
}
