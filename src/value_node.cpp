#include "value_node.hpp"

#include "node.hpp"
#include "node_types.hpp"

#include <iostream>

libconfigfile::value_node::~value_node() {}

libconfigfile::node_type libconfigfile::value_node::get_node_type() const {
  return node_type::VALUE;
}
