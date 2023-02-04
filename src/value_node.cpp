#include "value_node.hpp"

#include "node.hpp"
#include "node_types.hpp"

#include <cstddef>
#include <utility>

libconfigfile::value_node::~value_node() {}

libconfigfile::actual_node_type
libconfigfile::value_node::get_actual_node_type() const {
  return actual_node_type::VALUE_NODE;
}

libconfigfile::node_type libconfigfile::value_node::get_node_type() const {
  return node_type::VALUE;
}
