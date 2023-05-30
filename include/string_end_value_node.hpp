#ifndef LIBCONFIGFILE_STRING_END_VALUE_NODE
#define LIBCONFIGFILE_STRING_END_VALUE_NODE

#include "end_value_node.hpp"
#include "node_types.hpp"

#include <iostream>
#include <string>
#include <type_traits>

namespace libconfigfile {

class string_end_value_node : public end_value_node, std::string {
private:
  using base_t = std::string;

public:
  using base_t::base_t;
  string_end_value_node();
  string_end_value_node(const string_end_value_node &other);
  string_end_value_node(string_end_value_node &&other) noexcept(
      std::is_nothrow_move_constructible_v<base_t>);
  string_end_value_node(const base_t &other);
  string_end_value_node(base_t &&other) noexcept(
      std::is_nothrow_move_constructible_v<base_t>);

  virtual ~string_end_value_node() override;

public:
  virtual string_end_value_node *create_new() const override;
  virtual string_end_value_node *create_clone() const override;
  virtual absolute_node_type get_absolute_node_type() const override;
  virtual end_value_node_type get_end_value_node_type() const override final;
  virtual bool polymorphic_value_compare(const node *other) const override;
  virtual std::ostream &print(std::ostream &out) const override;

public:
  string_end_value_node &operator=(const string_end_value_node &other);
  string_end_value_node &operator=(string_end_value_node &&other) noexcept(
      std::is_nothrow_move_assignable_v<base_t>);
  string_end_value_node &operator=(const base_t &other);
  string_end_value_node &
  operator=(base_t &&other) noexcept(std::is_nothrow_move_assignable_v<base_t>);

public:
  friend std::ostream &operator<<(std::ostream &out,
                                  const string_end_value_node &n);
};

std::ostream &operator<<(std::ostream &out, const string_end_value_node &n);
} // namespace libconfigfile

#endif
