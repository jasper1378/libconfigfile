#include "node.hpp"

#include "node_types.hpp"

#include <cstddef>
#include <utility>

conf_file::node::~node()
{
}

conf_file::actual_node_type conf_file::node::get_actual_node_type() const
{
    return actual_node_type::NODE;
}
