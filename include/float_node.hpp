#ifndef LIBCONFIGFILE_FLOAT_NODE
#define LIBCONFIGFILE_FLOAT_NODE

#include "node.hpp"
#include "node_types.hpp"

#include <iostream>
#include <limits>
#include <type_traits>

namespace libconfigfile {

using float_node_data_t = double;

class float_node : public node {
public:
  using value_t = float_node_data_t;
  static_assert(std::is_floating_point_v<value_t>);
  static_assert(std::numeric_limits<float_node_data_t>::has_infinity);
  static_assert(std::numeric_limits<float_node_data_t>::has_quiet_NaN);

private:
  value_t m_value;

public:
  float_node();
  float_node(const value_t value);
  float_node(const float_node &other);
  float_node(float_node &&other) noexcept(
      std::is_nothrow_move_constructible_v<value_t>);

  virtual ~float_node() override;

public:
  virtual float_node *create_new() const override;
  virtual float_node *create_clone() const override;
  virtual libconfigfile::node_type get_node_type() const override final;
  virtual bool polymorphic_value_compare(const node *other) const override;
  virtual std::ostream &print(std::ostream &out) const override;

public:
  value_t get() const;
  void set(const value_t value);

  float_node &operator=(const float_node &other);
  float_node &operator=(float_node &&other) noexcept(
      std::is_nothrow_move_assignable_v<value_t>);
  float_node &operator=(const value_t value);

public:
  friend bool operator==(const float_node &x, const float_node &y);
  friend bool operator!=(const float_node &x, const float_node &y);

  friend std::ostream &operator<<(std::ostream &out, const float_node &n);
};

bool operator==(const float_node &x, const float_node &y);
bool operator!=(const float_node &x, const float_node &y);

std::ostream &operator<<(std::ostream &out, const float_node &n);
} // namespace libconfigfile

#endif
