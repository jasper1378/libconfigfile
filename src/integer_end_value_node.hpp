#ifndef CONF_FILE_INTEGER_END_VALUE_NODE_HPP
#define CONF_FILE_INTEGER_END_VALUE_NODE_HPP

#include "end_value_node.hpp"
#include "node.hpp"
#include "node_types.hpp"
#include "value_node.hpp"

#include <cstddef>
#include <cstdint>
#include <iostream>

namespace conf_file
{
    class integer_end_value_node : public end_value_node
    {
        public:

            using value_t = int64_t;

        private:

            value_t m_value;

        public:

            integer_end_value_node();
            explicit integer_end_value_node(const value_t value);
            integer_end_value_node(const integer_end_value_node& other);
            integer_end_value_node(integer_end_value_node&& other);

            virtual ~integer_end_value_node() override;

        public:

            virtual actual_node_type get_actual_node_type() const override;

            virtual integer_end_value_node* create_new() const override;
            virtual integer_end_value_node* create_clone() const override;
            virtual end_value_node_type get_end_value_node_type() const override final;

            value_t get() const;
            value_t& get();
            void set(const value_t value);

        public:

            integer_end_value_node& operator=(const integer_end_value_node& other);
            integer_end_value_node& operator=(integer_end_value_node&& other);
            integer_end_value_node& operator=(const value_t value);

            explicit operator value_t() const;

        public:

            friend std::ostream& operator<<(std::ostream& out, const integer_end_value_node& i);
            friend std::istream& operator>>(std::istream& in, integer_end_value_node& i);
    };

    std::ostream& operator<<(std::ostream& out, const integer_end_value_node& i);
    std::istream& operator>>(std::istream& in, integer_end_value_node& i);
}

#endif
