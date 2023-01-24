#ifndef CONF_FILE_NODE_HPP
#define CONF_FILE_NODE_HPP

#include "node_types.hpp"

#include <cstddef>

namespace libconfigfile
{
    class node
    {
        public:

            virtual ~node();

        public:

            virtual actual_node_type get_actual_node_type() const;

            virtual node* create_new() const =0;
            virtual node* create_clone() const =0;
            virtual node_type get_node_type() const =0;
    };
}

#endif
