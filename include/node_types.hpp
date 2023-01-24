#ifndef CONF_FILE_NODE_TYPES_HPP
#define CONF_FILE_NODE_TYPES_HPP

namespace conf_file
{
    enum class actual_node_type
    {
        NODE,
        SECTION_NODE,
        VALUE_NODE,
        ARRAY_VALUE_NODE,
        END_VALUE_NODE,
        INTEGER_END_VALUE_NODE,
        FLOAT_END_VALUE_NODE,
        STRING_END_VALUE_NODE,
        MAX,
    };

    enum class node_type
    {
        VALUE,
        SECTION,
        MAX,
    };

    enum class value_node_type
    {
        END_VALUE,
        ARRAY,
        MAX,
    };

    enum class end_value_node_type
    {
        INTEGER,
        FLOAT,
        STRING,
        MAX,
    };
}

#endif
