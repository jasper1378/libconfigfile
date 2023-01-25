#ifndef LIBCONFIGFILE_STRING_END_VALUE_NODE_HPP
#define LIBCONFIGFILE_STRING_END_VALUE_NODE_HPP

#include "end_value_node.hpp"
#include "node.hpp"
#include "node_types.hpp"
#include "value_node.hpp"

#include <cstddef>
#include <iostream>
#include <string>

namespace libconfigfile
{
    class string_end_value_node : public end_value_node
    {
        public:

            using value_t = std::string;

        private:

            value_t m_value;

        public:

            string_end_value_node();
            explicit string_end_value_node(const value_t& value);
            explicit string_end_value_node(value_t&& value);
            string_end_value_node(const string_end_value_node& other);
            string_end_value_node(string_end_value_node&& other);

            virtual ~string_end_value_node() override;

        public:

            virtual actual_node_type get_actual_node_type() const override;

            virtual string_end_value_node* create_new() const override;
            virtual string_end_value_node* create_clone() const override;
            virtual end_value_node_type get_end_value_node_type() const override final;

            const value_t& get() const;
            value_t& get();
            void set(const value_t& value);
            void set(value_t&& value);

        public:

            string_end_value_node& operator=(const string_end_value_node& other);
            string_end_value_node& operator=(string_end_value_node&& other);
            string_end_value_node& operator=(const value_t& value);
            string_end_value_node& operator=(value_t&& value);

            explicit operator value_t() const;

        public:

            friend std::ostream& operator<<(std::ostream& out, const string_end_value_node& s);
            friend std::istream& operator>>(std::istream& in, string_end_value_node& s);
    };

    std::ostream& operator<<(std::ostream& out, const string_end_value_node& s);
    std::istream& operator>>(std::istream& in, string_end_value_node& s);
}

#endif
