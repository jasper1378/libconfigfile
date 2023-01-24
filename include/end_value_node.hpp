#ifndef CONF_FILE_END_VALUE_NODE_HPP
#define CONF_FILE_END_VALUE_NODE_HPP

#include "node.hpp"
#include "node_types.hpp"
#include "value_node.hpp"

#include <cstddef>

namespace libconfigfile
{
    class end_value_node : public value_node
    {
        public:

            virtual ~end_value_node() override;

        public:

            virtual actual_node_type get_actual_node_type() const override;

            virtual end_value_node* create_new() const override =0;
            virtual end_value_node* create_clone() const override =0;
            virtual value_node_type get_value_node_type() const override final;

            virtual end_value_node_type get_end_value_node_type() const =0;
    };
}

#endif
