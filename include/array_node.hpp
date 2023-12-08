#ifndef LIBCONFIGFILE_Array_NODE_HPP
#define LIBCONFIGFILE_Array_NODE_HPP

#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"

#include <iostream>
#include <type_traits>
#include <vector>

namespace libconfigfile {
class array_node : public node, public std::vector<node_ptr<node, true>> {
public:
  using base_t = std::vector<node_ptr<node, true>>;

public:
  using base_t::base_t;
  array_node();
  array_node(const array_node &other);
  array_node(array_node &&other) noexcept(
      std::is_nothrow_move_constructible_v<base_t>);
  array_node(const base_t &other);
  array_node(base_t &&other) noexcept(
      std::is_nothrow_move_constructible_v<base_t>);

  virtual ~array_node() override;

public:
  virtual array_node *create_new() const override;
  virtual array_node *create_clone() const override;
  virtual libconfigfile::node_type get_node_type() const override final;
  virtual bool polymorphic_value_compare(const node *other) const override;
  virtual std::ostream &
  print(std::ostream &out,
        [[maybe_unused]] const int indent_level = 0) const override;

public:
  array_node &operator=(const array_node &other);
  array_node &operator=(array_node &&other) noexcept(
      std::is_nothrow_move_assignable_v<base_t>);
  array_node &operator=(const base_t &other);
  array_node &
  operator=(base_t &&other) noexcept(std::is_nothrow_move_assignable_v<base_t>);

public:
  friend std::ostream &operator<<(std::ostream &out, const array_node &n);

  friend array_node::base_t node_to_base(const array_node &node);
  friend array_node::base_t node_to_base(array_node &&node);
};

std::ostream &operator<<(std::ostream &out, const array_node &n);

array_node::base_t node_to_base(const array_node &node);
array_node::base_t node_to_base(array_node &&node);
} // namespace libconfigfile

#endif
