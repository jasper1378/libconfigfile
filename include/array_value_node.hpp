#ifndef LIBCONFIGFILE_ARRAY_VALUE_NODE_HPP
#define LIBCONFIGFILE_ARRAY_VALUE_NODE_HPP

#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"
#include "value_node.hpp"

#include <initializer_list>
#include <type_traits>
#include <utility>
#include <vector>

namespace libconfigfile {
class array_value_node : public value_node {
public:
  using vector_t = std::vector<node_ptr<value_node>>;
  using value_type = vector_t::value_type;
  using allocator_type = vector_t::allocator_type;
  using size_type = vector_t::size_type;
  using difference_type = vector_t::difference_type;
  using reference = vector_t::reference;
  using const_reference = vector_t::const_reference;
  using pointer = vector_t::pointer;
  using const_pointer = vector_t::const_pointer;
  using iterator = vector_t::iterator;
  using const_iterator = vector_t::const_iterator;
  using reverse_iterator = vector_t::reverse_iterator;
  using const_reverse_iterator = vector_t::const_reverse_iterator;

private:
  value_t m_value;

public:
  array_value_node();
  explicit array_value_node(const value_t &value);
  explicit array_value_node(value_t &&value);
  array_value_node(const array_value_node &other);
  array_value_node(array_value_node &&other);

  virtual ~array_value_node();

public:
  virtual actual_node_type get_actual_node_type() const override;

  virtual array_value_node *create_new() const override;
  virtual array_value_node *create_clone() const override;
  virtual value_node_type get_value_node_type() const override final;

  const value_t &get() const;
  value_t &get();
  void set(const value_t &value);
  void set(value_t &&value);

public:
  array_value_node &operator=(const array_value_node &other);
  array_value_node &operator=(array_value_node &&other);
  array_value_node &operator=(const value_t &value);
  array_value_node &operator=(value_t &&value);

  explicit operator value_t() const;
};
} // namespace libconfigfile

#endif
