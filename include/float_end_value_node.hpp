#ifndef LIBCONFIGFILE_FLOAT_END_VALUE_NODE_HPP
#define LIBCONFIGFILE_FLOAT_END_VALUE_NODE_HPP

#include "end_value_node.hpp"
#include "node.hpp"
#include "node_types.hpp"
#include "value_node.hpp"

#include <cstddef>
#include <iostream>

namespace libconfigfile {
class float_end_value_node : public end_value_node {
public:
  using value_t = double;

private:
  value_t m_value;

public:
  float_end_value_node();
  explicit float_end_value_node(const value_t value);
  float_end_value_node(const float_end_value_node &other);
  float_end_value_node(float_end_value_node &&other) noexcept;

  virtual ~float_end_value_node() override;

public:
  virtual actual_node_type get_actual_node_type() const override;

  virtual float_end_value_node *create_new() const override;
  virtual float_end_value_node *create_clone() const override;
  virtual end_value_node_type get_end_value_node_type() const override final;

  value_t get() const;
  value_t &get();
  void set(const value_t value);

public:
  float_end_value_node &operator=(const float_end_value_node &other);
  float_end_value_node &operator=(float_end_value_node &&other) noexcept;
  float_end_value_node &operator=(const value_t value);

  explicit operator value_t() const;

public:
  friend std::ostream &operator<<(std::ostream &out,
                                  const float_end_value_node &f);
  friend std::istream &operator>>(std::istream &in, float_end_value_node &f);
};

std::ostream &operator<<(std::ostream &out, const float_end_value_node &f);
std::istream &operator>>(std::istream &in, float_end_value_node &f);
} // namespace libconfigfile

#endif
