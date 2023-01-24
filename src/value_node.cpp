#include "value_node.hpp"

#include "node.hpp"
#include "node_types.hpp"

#include <cstddef>
#include <utility>

conf_file::value_node::~value_node()
{
}

conf_file::actual_node_type conf_file::value_node::get_actual_node_type() const
{
    return actual_node_type::VALUE_NODE;
}

conf_file::node_type conf_file::value_node::get_node_type() const
{
    return node_type::VALUE;
}
