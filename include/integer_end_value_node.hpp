#ifndef LIBCONFIGFILE_INTEGER_END_VALUE_NODE
#define LIBCONFIGFILE_INTEGER_END_VALUE_NODE

#include "character_constants.hpp"
#include "end_value_node.hpp"
#include "node_types.hpp"
#include "numeral_system.hpp"

#include <cstdint>
#include <iostream>
#include <type_traits>
#include <utility>

namespace libconfigfile {

using integer_end_value_node_data_t = int64_t;

class integer_end_value_node : public end_value_node {
public:
  using value_t = integer_end_value_node_data_t;
  static_assert(std::is_integral_v<value_t>);

private:
  value_t m_value;
  const numeral_system *m_num_sys;

public:
  integer_end_value_node();
  integer_end_value_node(
      value_t value, const numeral_system *num_sys = &numeral_system_decimal);
  integer_end_value_node(const integer_end_value_node &other);
  integer_end_value_node(integer_end_value_node &&other) noexcept(
      std::is_nothrow_move_constructible_v<value_t>);

  virtual ~integer_end_value_node() override;

public:
  virtual integer_end_value_node *create_new() const override;
  virtual integer_end_value_node *create_clone() const override;
  virtual absolute_node_type get_absolute_node_type() const override;
  virtual end_value_node_type get_end_value_node_type() const override final;
  virtual bool polymorphic_value_compare(const node *other) const override;
  virtual std::ostream &print(std::ostream &out) const override;

public:
  value_t get() const;
  void set(value_t value);

  const numeral_system *get_num_sys() const;
  void set_num_sys(const numeral_system *num_sys);

  std::pair<value_t, const numeral_system *> get_both() const;
  void set_both(const std::pair<value_t, const numeral_system *> &both);

  integer_end_value_node &operator=(const integer_end_value_node &other);
  integer_end_value_node &operator=(integer_end_value_node &&other) noexcept(
      std::is_nothrow_move_assignable_v<value_t>);
  integer_end_value_node &operator=(value_t value);
  integer_end_value_node &
  operator=(const std::pair<value_t, const numeral_system *> &both);

public:
  friend bool operator==(const integer_end_value_node &x,
                         const integer_end_value_node &y);
  friend bool operator!=(const integer_end_value_node &x,
                         const integer_end_value_node &y);

  friend std::ostream &operator<<(std::ostream &out,
                                  const integer_end_value_node &n);
};

bool operator==(const integer_end_value_node &x,
                const integer_end_value_node &y);
bool operator!=(const integer_end_value_node &x,
                const integer_end_value_node &y);

std::ostream &operator<<(std::ostream &out, const integer_end_value_node &n);
} // namespace libconfigfile

#endif
