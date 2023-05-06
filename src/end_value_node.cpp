#include "end_value_node.hpp"

#include "node_types.hpp"
#include "value_node.hpp"

#include <iostream>

libconfigfile::end_value_node::~end_value_node() {}

libconfigfile::value_node_type
libconfigfile::end_value_node::get_value_node_type() const {
  return value_node_type::END_VALUE;
}
