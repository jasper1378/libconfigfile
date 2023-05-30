#ifndef LIBCONFIGFILE_SECTION_NODE_HPP
#define LIBCONFIGFILE_SECTION_NODE_HPP

#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"

#include <iostream>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace libconfigfile {
class section_node
    : public node,
      public std::unordered_map<std::string, node_ptr<node, true>> {
private:
  using base_t = std::unordered_map<std::string, node_ptr<node, true>>;

public:
  using base_t::base_t;
  section_node();
  section_node(const section_node &other);
  section_node(section_node &&other) noexcept(
      std::is_nothrow_move_constructible_v<base_t>);
  section_node(const base_t &other);
  section_node(base_t &&other) noexcept(
      std::is_nothrow_move_constructible_v<base_t>);

  virtual ~section_node() override;

public:
  virtual section_node *create_new() const override;
  virtual section_node *create_clone() const override;
  virtual absolute_node_type get_absolute_node_type() const override;
  virtual libconfigfile::node_type get_node_type() const override final;
  virtual bool polymorphic_value_compare(const node *other) const override;
  virtual std::ostream &print(std::ostream &out) const override;

public:
  section_node &operator=(const section_node &other);
  section_node &operator=(section_node &&other) noexcept(
      std::is_nothrow_move_assignable_v<base_t>);
  section_node &operator=(const base_t &other);
  section_node &
  operator=(base_t &&other) noexcept(std::is_nothrow_move_assignable_v<base_t>);

public:
  friend std::ostream &operator<<(std::ostream &out, const section_node &n);
};

std::ostream &operator<<(std::ostream &out, const section_node &n);
} // namespace libconfigfile

#endif
