#include "section_node.hpp"

#include "node.hpp"
#include "node_types.hpp"

#include <cstddef>
#include <string>
#include <unordered_map>
#include <utility>

libconfigfile::section_node::section_node()
    : m_name{},
      m_value{}
{
}

libconfigfile::section_node::section_node(const std::string& name, const value_t& value)
    : m_name{ name },
      m_value{ value }
{
}

libconfigfile::section_node::section_node(const std::string& name, value_t&& value)
    : m_name{ name },
      m_value{ std::move(value) }
{
}

libconfigfile::section_node::section_node(std::string&& name, const value_t& value)
    : m_name{ std::move(name) },
      m_value{ value }
{
}

libconfigfile::section_node::section_node(std::string&& name, value_t&& value)
    : m_name{ std::move(name) },
      m_value{ std::move(value) }
{
}

libconfigfile::section_node::section_node(const section_node& other)
    : m_name{ other.m_name },
      m_value{ other.m_value }
{
}

libconfigfile::section_node::section_node(section_node&& other)
    : m_name{ std::move(other.m_name) },
      m_value{ std::move(other.m_value) }
{
}

libconfigfile::section_node::~section_node()
{
}

libconfigfile::actual_node_type libconfigfile::section_node::get_actual_node_type() const
{
    return actual_node_type::SECTION_NODE;
}

libconfigfile::section_node* libconfigfile::section_node::create_new() const
{
    return new section_node{};
}

libconfigfile::section_node* libconfigfile::section_node::create_clone() const
{
    return new section_node{ *this };
}

libconfigfile::node_type libconfigfile::section_node::get_node_type() const
{
    return node_type::SECTION;
}

const std::string& libconfigfile::section_node::get_name() const
{
    return m_name;
}

std::string& libconfigfile::section_node::get_name()
{
    return m_name;
}

void libconfigfile::section_node::set_name(const std::string& name)
{
    m_name = name;
}

void libconfigfile::section_node::set_name(std::string&& name)
{
    m_name = std::move(name);
}

const libconfigfile::section_node::value_t& libconfigfile::section_node::get() const
{
    return m_value;
}

libconfigfile::section_node::value_t& libconfigfile::section_node::get()
{
    return m_value;
}

void libconfigfile::section_node::set(const value_t& value)
{
    m_value = value;
}

void libconfigfile::section_node::set(value_t&& value)
{
    m_value = std::move(value);
}

libconfigfile::section_node& libconfigfile::section_node::operator=(const section_node& other)
{
    if (this == &other)
    {
        return *this;
    }

    m_name = other.m_name;
    m_value = other.m_value;

    return *this;
}

libconfigfile::section_node& libconfigfile::section_node::operator=(section_node&& other)
{
    if (this == &other)
    {
        return *this;
    }

    m_name = std::move(other.m_name);
    m_value = std::move(other.m_value);

    return *this;
}

libconfigfile::section_node& libconfigfile::section_node::operator=(const value_t& other)
{
    m_value = other;

    return *this;
}

libconfigfile::section_node& libconfigfile::section_node::operator=(value_t&& other)
{
    m_value = std::move(other);

    return *this;
}

libconfigfile::section_node::operator value_t() const
{
    return m_value;
}
