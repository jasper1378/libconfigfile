#ifndef LIBCONFIGFILE_INTEGER_NODE
#define LIBCONFIGFILE_INTEGER_NODE

#include "character_constants.hpp"
#include "node.hpp"
#include "node_types.hpp"
#include "numeral_system.hpp"

#include <cstdint>
#include <iostream>
#include <type_traits>
#include <utility>

namespace libconfigfile {

using integer_node_data_t = int64_t;

class integer_node : public node {
public:
  using value_t = integer_node_data_t;
  static_assert(std::is_integral_v<value_t>);

private:
  value_t m_value;
  const numeral_system *m_num_sys;

public:
  integer_node();
  integer_node(const value_t value,
               const numeral_system *num_sys = &numeral_system_decimal);
  integer_node(const integer_node &other);
  integer_node(integer_node &&other) noexcept(
      std::is_nothrow_move_constructible_v<value_t>);

  virtual ~integer_node() override;

public:
  virtual integer_node *create_new() const override;
  virtual integer_node *create_clone() const override;
  virtual libconfigfile::node_type get_node_type() const override final;
  virtual bool polymorphic_value_compare(const node *other) const override;
  virtual std::ostream &
  print(std::ostream &out,
        [[maybe_unused]] const int indent_level = 0) const override;

public:
  value_t get() const;
  void set(const value_t value);

  const numeral_system *get_num_sys() const;
  void set_num_sys(const numeral_system *num_sys);

  std::pair<value_t, const numeral_system *> get_both() const;
  void set_both(const std::pair<value_t, const numeral_system *> &both);

  integer_node &operator=(const integer_node &other);
  integer_node &operator=(integer_node &&other) noexcept(
      std::is_nothrow_move_assignable_v<value_t>);
  integer_node &operator=(const value_t value);
  integer_node &
  operator=(const std::pair<value_t, const numeral_system *> &both);

public:
  friend bool operator==(const integer_node &x, const integer_node &y);
  friend bool operator!=(const integer_node &x, const integer_node &y);

  friend std::ostream &operator<<(std::ostream &out, const integer_node &n);
};

bool operator==(const integer_node &x, const integer_node &y);
bool operator!=(const integer_node &x, const integer_node &y);

std::ostream &operator<<(std::ostream &out, const integer_node &n);
} // namespace libconfigfile

#endif
