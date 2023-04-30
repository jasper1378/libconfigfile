#ifndef LIBCONFIGFILE_INTEGER_END_VALUE_NODE
#define LIBCONFIGFILE_INTEGER_END_VALUE_NODE

#include "end_value_node.hpp"
#include "node_types.hpp"

#include <cstdint>
#include <type_traits>

namespace libconfigfile {

using integer_end_value_node_data_t = int64_t;

class integer_end_value_node : public end_value_node {
public:
  using value_t = integer_end_value_node_data_t;

private:
  value_t m_value;

public:
  integer_end_value_node();
  integer_end_value_node(const value_t &value);
  integer_end_value_node(value_t &&value);
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

  const value_t &get() const;
  value_t &get();
  void set(const value_t &value);
  void set(value_t &&value);

  integer_end_value_node &operator=(const integer_end_value_node &other);
  integer_end_value_node &operator=(integer_end_value_node &&other) noexcept(
      std::is_nothrow_move_assignable_v<value_t>);
  integer_end_value_node &operator=(const value_t &value);
  integer_end_value_node &operator=(value_t &&value);

public:
  friend bool operator==(const integer_end_value_node &x,
                         const integer_end_value_node &y);
  friend bool operator!=(const integer_end_value_node &x,
                         const integer_end_value_node &y);
};

bool operator==(const integer_end_value_node &x,
                const integer_end_value_node &y);
bool operator!=(const integer_end_value_node &x,
                const integer_end_value_node &y);
} // namespace libconfigfile

#endif
