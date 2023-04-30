#ifndef LIBCONFIGFILE_FLOAT_END_VALUE_NODE
#define LIBCONFIGFILE_FLOAT_END_VALUE_NODE

#include "end_value_node.hpp"
#include "node_types.hpp"

#include <type_traits>

namespace libconfigfile {

using float_end_value_node_data_t = double;

class float_end_value_node : public end_value_node {
public:
  using value_t = float_end_value_node_data_t;

private:
  value_t m_value;

public:
  float_end_value_node();
  float_end_value_node(const value_t &value);
  float_end_value_node(value_t &&value);
  float_end_value_node(const float_end_value_node &other);
  float_end_value_node(float_end_value_node &&other) noexcept(
      std::is_nothrow_move_constructible_v<value_t>);

  virtual ~float_end_value_node() override;

public:
  virtual float_end_value_node *create_new() const override;
  virtual float_end_value_node *create_clone() const override;
  virtual absolute_node_type get_absolute_node_type() const override;
  virtual end_value_node_type get_end_value_node_type() const override final;
  virtual bool polymorphic_value_compare(const node *other) const override;

  const value_t &get() const;
  value_t &get();
  void set(const value_t &value);
  void set(value_t &&value);

  float_end_value_node &operator=(const float_end_value_node &other);
  float_end_value_node &operator=(float_end_value_node &&other) noexcept(
      std::is_nothrow_move_assignable_v<value_t>);
  float_end_value_node &operator=(const value_t &value);
  float_end_value_node &operator=(value_t &&value);

public:
  friend bool operator==(const float_end_value_node &x,
                         const float_end_value_node &y);
  friend bool operator!=(const float_end_value_node &x,
                         const float_end_value_node &y);
};

bool operator==(const float_end_value_node &x, const float_end_value_node &y);
bool operator!=(const float_end_value_node &x, const float_end_value_node &y);

} // namespace libconfigfile

#endif
