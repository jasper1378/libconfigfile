#include "end_value_node.hpp"

#include "node.hpp"
#include "node_types.hpp"
#include "value_node.hpp"

#include <cstddef>
#include <utility>

conf_file::end_value_node::~end_value_node()
{
}

conf_file::actual_node_type conf_file::end_value_node::get_actual_node_type() const
{
    return actual_node_type::END_VALUE_NODE;
}

conf_file::value_node_type conf_file::end_value_node::get_value_node_type() const
{
    return value_node_type::END_VALUE;
}
