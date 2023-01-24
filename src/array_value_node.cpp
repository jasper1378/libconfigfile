#include "array_value_node.hpp"

#include "node.hpp"
#include "node_types.hpp"
#include "value_node.hpp"

#include <cstddef>
#include <utility>
#include <vector>

conf_file::array_value_node::array_value_node()
    : m_value{}
{
}

conf_file::array_value_node::array_value_node(const value_t& value)
    : m_value{ value }
{
}

conf_file::array_value_node::array_value_node(value_t&& value)
    : m_value{ std::move(value) }
{
}

conf_file::array_value_node::array_value_node(const array_value_node& other)
    : m_value{ other.m_value }
{
}

conf_file::array_value_node::array_value_node(array_value_node&& other)
    : m_value{ std::move(other.m_value) }
{
}

conf_file::array_value_node::~array_value_node()
{
}

conf_file::actual_node_type conf_file::array_value_node::get_actual_node_type() const
{
    return actual_node_type::ARRAY_VALUE_NODE;
}

conf_file::array_value_node* conf_file::array_value_node::create_new() const
{
    return new array_value_node{};
}

conf_file::array_value_node* conf_file::array_value_node::create_clone() const
{
    return new array_value_node{ *this };
}

conf_file::value_node_type conf_file::array_value_node::get_value_node_type() const
{
    return value_node_type::ARRAY;
}

const conf_file::array_value_node::value_t& conf_file::array_value_node::get() const
{
    return m_value;
}

conf_file::array_value_node::value_t& conf_file::array_value_node::get()
{
    return m_value;
}

void conf_file::array_value_node::set(const value_t& value)
{
    m_value = value;
}

void conf_file::array_value_node::set(value_t&& value)
{
    m_value = std::move(value);
}

conf_file::array_value_node& conf_file::array_value_node::operator=(const array_value_node& other)
{
    if (this == &other)
    {
        return *this;
    }

    m_value = other.m_value;

    return *this;
}

conf_file::array_value_node& conf_file::array_value_node::operator=(array_value_node&& other)
{
    if (this == &other)
    {
        return *this;
    }

    m_value = std::move(other.m_value);

    return *this;
}


conf_file::array_value_node& conf_file::array_value_node::operator=(const value_t& value)
{
    m_value = value;

    return *this;
}

conf_file::array_value_node& conf_file::array_value_node::operator=(value_t&& value)
{
    m_value = std::move(value);

    return *this;
}

conf_file::array_value_node::operator value_t() const
{
    return m_value;
}
