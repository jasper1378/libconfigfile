#include "node.hpp"

#include "node_types.hpp"

#include <cstddef>
#include <utility>

libconfigfile::node::~node() {}

libconfigfile::actual_node_type
libconfigfile::node::get_actual_node_type() const {
  return actual_node_type::NODE;
}
