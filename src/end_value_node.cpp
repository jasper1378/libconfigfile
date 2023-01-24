#include "end_value_node.hpp"

#include "node.hpp"
#include "node_types.hpp"
#include "value_node.hpp"

#include <cstddef>
#include <utility>

libconfigfile::end_value_node::~end_value_node()
{
}

libconfigfile::actual_node_type libconfigfile::end_value_node::get_actual_node_type() const
{
    return actual_node_type::END_VALUE_NODE;
}

libconfigfile::value_node_type libconfigfile::end_value_node::get_value_node_type() const
{
    return value_node_type::END_VALUE;
}
