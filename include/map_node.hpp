#ifndef LIBCONFIGFILE_MAP_NODE_HPP
#define LIBCONFIGFILE_MAP_NODE_HPP

#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"

#include <iostream>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace libconfigfile {
class map_node : public node,
                 public std::unordered_map<std::string, node_ptr<node, true>> {
public:
  using base_t = std::unordered_map<std::string, node_ptr<node, true>>;

private:
  bool m_is_root_map{false};

public:
  using base_t::base_t;
  map_node();
  map_node(const map_node &other);
  map_node(map_node &&other) noexcept(
      std::is_nothrow_move_constructible_v<base_t>);
  map_node(const base_t &other);
  map_node(base_t &&other) noexcept(
      std::is_nothrow_move_constructible_v<base_t>);

  virtual ~map_node() override;

public:
  virtual map_node *create_new() const override;
  virtual map_node *create_clone() const override;
  virtual libconfigfile::node_type get_node_type() const override final;
  virtual bool polymorphic_value_compare(const node *other) const override;
  virtual std::string serialize(int indent_level = 0) const override;
  virtual std::ostream &print(std::ostream &out,
                              const int indent_level = 0) const override;

public:
  bool get_is_root_map() const;
  void set_is_root_map(const bool is_root_map);

public:
  map_node &operator=(const map_node &other);
  map_node &operator=(map_node &&other) noexcept(
      std::is_nothrow_move_assignable_v<base_t>);
  map_node &operator=(const base_t &other);
  map_node &
  operator=(base_t &&other) noexcept(std::is_nothrow_move_assignable_v<base_t>);

public:
  friend std::ostream &operator<<(std::ostream &out, const map_node &n);

  friend map_node::base_t node_to_base(const map_node &node);
  friend map_node::base_t node_to_base(map_node &&node);
};

std::ostream &operator<<(std::ostream &out, const map_node &n);

map_node::base_t node_to_base(const map_node &node);
map_node::base_t node_to_base(map_node &&node);
} // namespace libconfigfile

#endif
