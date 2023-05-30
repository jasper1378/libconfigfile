#ifndef LIBCONFIGFILE_ARRAY_VALUE_NODE_HPP
#define LIBCONFIGFILE_ARRAY_VALUE_NODE_HPP

#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"
#include "value_node.hpp"

#include <iostream>
#include <type_traits>
#include <vector>

namespace libconfigfile {
class array_value_node : public value_node,
                         public std::vector<node_ptr<value_node, true>> {
private:
  using base_t = std::vector<node_ptr<value_node, true>>;

public:
  using base_t::base_t;
  array_value_node();
  array_value_node(const array_value_node &other);
  array_value_node(array_value_node &&other) noexcept(
      std::is_nothrow_move_constructible_v<base_t>);
  array_value_node(const base_t &other);
  array_value_node(base_t &&other) noexcept(
      std::is_nothrow_move_constructible_v<base_t>);

  virtual ~array_value_node() override;

public:
  virtual array_value_node *create_new() const override;
  virtual array_value_node *create_clone() const override;
  virtual absolute_node_type get_absolute_node_type() const override;
  virtual value_node_type get_value_node_type() const override final;
  virtual bool polymorphic_value_compare(const node *other) const override;
  virtual std::ostream &print(std::ostream &out) const override;

public:
  array_value_node &operator=(const array_value_node &other);
  array_value_node &operator=(array_value_node &&other) noexcept(
      std::is_nothrow_move_assignable_v<base_t>);
  array_value_node &operator=(const base_t &other);
  array_value_node &
  operator=(base_t &&other) noexcept(std::is_nothrow_move_assignable_v<base_t>);

public:
  friend std::ostream &operator<<(std::ostream &out, const array_value_node &n);
};

std::ostream &operator<<(std::ostream &out, const array_value_node &n);
} // namespace libconfigfile

#endif
