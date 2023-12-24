#ifndef LIBCONFIGFILE_Integer_NODE
#define LIBCONFIGFILE_Integer_NODE

#include "character_constants.hpp"
#include "node.hpp"
#include "node_types.hpp"
#include "numeral_system.hpp"

#include <cstdint>
#include <iostream>
#include <type_traits>
#include <utility>

namespace libconfigfile {

class integer_node : public node {
public:
  using base_t = int64_t;

private:
  static_assert(std::is_integral_v<base_t>);

private:
  base_t m_value;
  const numeral_system *m_num_sys;

public:
  integer_node();
  integer_node(const base_t value,
               const numeral_system *num_sys = &numeral_system_decimal);
  integer_node(const integer_node &other);
  integer_node(integer_node &&other) noexcept(
      std::is_nothrow_move_constructible_v<base_t>);

  virtual ~integer_node() override;

public:
  virtual integer_node *create_new() const override;
  virtual integer_node *create_clone() const override;
  virtual libconfigfile::node_type get_node_type() const override final;
  virtual bool polymorphic_value_compare(const node *other) const override;
  virtual std::string serialize(int indent_level = 0) const override;
  virtual std::ostream &
  print(std::ostream &out,
        [[maybe_unused]] const int indent_level = 0) const override;

public:
  base_t get() const;
  void set(const base_t value);

  const numeral_system *get_num_sys() const;
  void set_num_sys(const numeral_system *num_sys);

  std::pair<base_t, const numeral_system *> get_both() const;
  void set_both(const std::pair<base_t, const numeral_system *> &both);

  integer_node &operator=(const integer_node &other);
  integer_node &operator=(integer_node &&other) noexcept(
      std::is_nothrow_move_assignable_v<base_t>);
  integer_node &operator=(const base_t value);
  integer_node &
  operator=(const std::pair<base_t, const numeral_system *> &both);

public:
  friend bool operator==(const integer_node &x, const integer_node &y);
  friend bool operator!=(const integer_node &x, const integer_node &y);

  friend std::ostream &operator<<(std::ostream &out, const integer_node &n);

  friend integer_node::base_t node_to_base(const integer_node &node);
  friend integer_node::base_t node_to_base(integer_node &&node);
};

bool operator==(const integer_node &x, const integer_node &y);
bool operator!=(const integer_node &x, const integer_node &y);

std::ostream &operator<<(std::ostream &out, const integer_node &n);

integer_node::base_t node_to_base(const integer_node &node);
integer_node::base_t node_to_base(integer_node &&node);
} // namespace libconfigfile

#endif
