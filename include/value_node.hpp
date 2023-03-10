#ifndef LIBCONFIGFILE_VALUE_NODE_HPP
#define LIBCONFIGFILE_VALUE_NODE_HPP

#include "node.hpp"
#include "node_types.hpp"

#include <cstddef>

namespace libconfigfile {
class value_node : public node {
public:
  virtual ~value_node() override;

public:
  virtual value_node *create_new() const override = 0;
  virtual value_node *create_clone() const override = 0;
  virtual node_type get_node_type() const override final;

  virtual value_node_type get_value_node_type() const = 0;
};
} // namespace libconfigfile

#endif
