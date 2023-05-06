#ifndef LIBCONFIGFILE_STRING_END_VALUE_NODE
#define LIBCONFIGFILE_STRING_END_VALUE_NODE

#include "end_value_node.hpp"
#include "node_types.hpp"

#include <iostream>
#include <string>
#include <type_traits>

namespace libconfigfile {

using string_end_value_node_data_t = std::string;

class string_end_value_node : public end_value_node {
public:
  using value_t = string_end_value_node_data_t;

private:
  value_t m_value;

public:
  string_end_value_node();
  string_end_value_node(const value_t &value);
  string_end_value_node(value_t &&value);
  string_end_value_node(const string_end_value_node &other);
  string_end_value_node(string_end_value_node &&other) noexcept(
      std::is_nothrow_move_constructible_v<value_t>);

  virtual ~string_end_value_node() override;

public:
  virtual string_end_value_node *create_new() const override;
  virtual string_end_value_node *create_clone() const override;
  virtual absolute_node_type get_absolute_node_type() const override;
  virtual end_value_node_type get_end_value_node_type() const override final;
  virtual bool polymorphic_value_compare(const node *other) const override;
  virtual void print(std::ostream &out) const override;

public:
  const value_t &get() const;
  value_t &get();
  void set(const value_t &value);
  void set(value_t &&value);

  string_end_value_node &operator=(const string_end_value_node &other);
  string_end_value_node &operator=(string_end_value_node &&other) noexcept(
      std::is_nothrow_move_assignable_v<value_t>);
  string_end_value_node &operator=(const value_t &value);
  string_end_value_node &operator=(value_t &&value);

public:
  friend bool operator==(const string_end_value_node &x,
                         const string_end_value_node &y);
  friend bool operator!=(const string_end_value_node &x,
                         const string_end_value_node &y);

  friend std::ostream &operator<<(std::ostream &out,
                                  const string_end_value_node &n);
};

bool operator==(const string_end_value_node &x, const string_end_value_node &y);
bool operator!=(const string_end_value_node &x, const string_end_value_node &y);

std::ostream &operator<<(std::ostream &out, const string_end_value_node &n);
} // namespace libconfigfile

#endif
