#ifndef LIBCONFIGFILE_String_NODE
#define LIBCONFIGFILE_String_NODE

#include "node.hpp"
#include "node_types.hpp"

#include <iostream>
#include <string>
#include <type_traits>

namespace libconfigfile {

class string_node : public node, public std::string {
public:
  using base_t = std::string;

public:
  using base_t::base_t;
  string_node();
  string_node(const string_node &other);
  string_node(string_node &&other) noexcept(
      std::is_nothrow_move_constructible_v<base_t>);
  string_node(const base_t &other);
  string_node(base_t &&other) noexcept(
      std::is_nothrow_move_constructible_v<base_t>);

  virtual ~string_node() override;

public:
  virtual string_node *create_new() const override;
  virtual string_node *create_clone() const override;
  virtual libconfigfile::node_type get_node_type() const override final;
  virtual bool polymorphic_value_compare(const node *other) const override;
  virtual std::ostream &
  print(std::ostream &out,
        [[maybe_unused]] const int indent_level = 0) const override;

public:
  string_node &operator=(const string_node &other);
  string_node &operator=(string_node &&other) noexcept(
      std::is_nothrow_move_assignable_v<base_t>);
  string_node &operator=(const base_t &other);
  string_node &
  operator=(base_t &&other) noexcept(std::is_nothrow_move_assignable_v<base_t>);

public:
  friend std::ostream &operator<<(std::ostream &out, const string_node &n);

  friend string_node::base_t node_to_base(const string_node &node);
  friend string_node::base_t node_to_base(string_node &&node);
};

std::ostream &operator<<(std::ostream &out, const string_node &n);

string_node::base_t node_to_base(const string_node &node);
string_node::base_t node_to_base(string_node &&node);
} // namespace libconfigfile

#endif
