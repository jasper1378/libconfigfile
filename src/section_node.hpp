#ifndef CONF_FILE_SECTION_NODE_HPP
#define CONF_FILE_SECTION_NODE_HPP

#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"

#include <cstddef>
#include <string>
#include <unordered_map>

namespace conf_file
{
    class section_node : public node
    {
        public:

            using value_t = std::unordered_map<std::string, node_ptr<node>>;

        private:

            std::string m_name;
            value_t m_value;

        public:

            section_node();
            explicit section_node(const std::string& name, const value_t& value);
            explicit section_node(const std::string& name, value_t&& value);
            explicit section_node(std::string&& name, const value_t& value);
            explicit section_node(std::string&& name, value_t&& value);
            section_node(const section_node& other);
            section_node(section_node&& other);

            virtual ~section_node() override;

        public:

            virtual actual_node_type get_actual_node_type() const override;

            virtual section_node* create_new() const override;
            virtual section_node* create_clone() const override;
            virtual node_type get_node_type() const override final;

            const std::string& get_name() const;
            std::string& get_name();
            void set_name(const std::string& name);
            void set_name(std::string&& name);
            const value_t& get() const;
            value_t& get();
            void set(const value_t& value);
            void set(value_t&& value);

        public:

            section_node& operator=(const section_node& other);
            section_node& operator=(section_node&& other);
            section_node& operator=(const value_t& value);
            section_node& operator=(value_t&& value);

            explicit operator value_t() const;
    };
}

#endif
