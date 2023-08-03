#ifndef LIBCONFIGFILE_ARRAY_NODE_HPP
#define LIBCONFIGFILE_ARRAY_NODE_HPP

#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"

#include <iostream>
#include <type_traits>
#include <vector>

namespace libconfigfile {
class array_node : public node, public std::vector<node_ptr<node, true>> {
private:
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
  virtual std::ostream &print(std::ostream &out) const override;

public:
  array_node &operator=(const array_node &other);
  array_node &operator=(array_node &&other) noexcept(
      std::is_nothrow_move_assignable_v<base_t>);
  array_node &operator=(const base_t &other);
  array_node &
  operator=(base_t &&other) noexcept(std::is_nothrow_move_assignable_v<base_t>);

public:
  friend std::ostream &operator<<(std::ostream &out, const array_node &n);
};

std::ostream &operator<<(std::ostream &out, const array_node &n);
} // namespace libconfigfile

#endif
