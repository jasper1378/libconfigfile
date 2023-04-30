#ifndef LIBCONFIGFILE_END_VALUE_NODE_HPP
#define LIBCONFIGFILE_END_VALUE_NODE_HPP

#include "node_types.hpp"
#include "value_node.hpp"

namespace libconfigfile {
class end_value_node : public value_node {
public:
  virtual ~end_value_node() override;

public:
  virtual end_value_node *create_new() const override = 0;
  virtual end_value_node *create_clone() const override = 0;
  virtual absolute_node_type get_absolute_node_type() const override = 0;
  virtual value_node_type get_value_node_type() const override final;
  virtual end_value_node_type get_end_value_node_type() const = 0;
  virtual bool polymorphic_value_compare(const node *other) const override = 0;
};
} // namespace libconfigfile

#endif
