#ifndef LIBCONFIGFILE_Float_NODE
#define LIBCONFIGFILE_Float_NODE

#include "node.hpp"
#include "node_types.hpp"

#include <iostream>
#include <limits>
#include <type_traits>

namespace libconfigfile {

class float_node : public node {
public:
  using base_t = double;

private:
  static_assert(std::is_floating_point_v<base_t>);
  static_assert(std::numeric_limits<base_t>::has_infinity);
  static_assert(std::numeric_limits<base_t>::has_quiet_NaN);

private:
  base_t m_value;

public:
  float_node();
  float_node(const base_t value);
  float_node(const float_node &other);
  float_node(float_node &&other) noexcept(
      std::is_nothrow_move_constructible_v<base_t>);

  virtual ~float_node() override;

public:
  virtual float_node *create_new() const override;
  virtual float_node *create_clone() const override;
  virtual libconfigfile::node_type get_node_type() const override final;
  virtual bool polymorphic_value_compare(const node *other) const override;
  virtual std::ostream &
  print(std::ostream &out,
        [[maybe_unused]] const int indent_level = 0) const override;

public:
  base_t get() const;
  void set(const base_t value);

  float_node &operator=(const float_node &other);
  float_node &operator=(float_node &&other) noexcept(
      std::is_nothrow_move_assignable_v<base_t>);
  float_node &operator=(const base_t value);

public:
  friend bool operator==(const float_node &x, const float_node &y);
  friend bool operator!=(const float_node &x, const float_node &y);

  friend std::ostream &operator<<(std::ostream &out, const float_node &n);

  friend float_node::base_t node_to_base(const float_node &node);
  friend float_node::base_t node_to_base(float_node &&node);
};

bool operator==(const float_node &x, const float_node &y);
bool operator!=(const float_node &x, const float_node &y);

std::ostream &operator<<(std::ostream &out, const float_node &n);

float_node::base_t node_to_base(const float_node &node);
float_node::base_t node_to_base(float_node &&node);
} // namespace libconfigfile

#endif
