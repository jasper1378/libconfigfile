#include "float_end_value_node.hpp"

#include "end_value_node.hpp"
#include "node.hpp"
#include "node_types.hpp"
#include "value_node.hpp"

#include <cstddef>
#include <iostream>
#include <utility>

conf_file::float_end_value_node::float_end_value_node()
    : m_value{}
{
}

conf_file::float_end_value_node::float_end_value_node(const value_t value)
    : m_value{ value }
{
}

conf_file::float_end_value_node::float_end_value_node(const float_end_value_node& other)
    : m_value{ other.m_value }
{
}

conf_file::float_end_value_node::float_end_value_node(float_end_value_node&& other)
    : m_value{ std::move(other.m_value) }
{
}

conf_file::float_end_value_node::~float_end_value_node()
{
}

conf_file::actual_node_type conf_file::float_end_value_node::get_actual_node_type() const
{
    return actual_node_type::FLOAT_END_VALUE_NODE;
}

conf_file::float_end_value_node* conf_file::float_end_value_node::create_new() const
{
    return new float_end_value_node{};
}

conf_file::float_end_value_node* conf_file::float_end_value_node::create_clone() const
{
    return new float_end_value_node{ *this };
}

conf_file::end_value_node_type conf_file::float_end_value_node::get_end_value_node_type() const
{
    return end_value_node_type::FLOAT;
}

conf_file::float_end_value_node::value_t conf_file::float_end_value_node::get() const
{
    return m_value;
}

conf_file::float_end_value_node::value_t& conf_file::float_end_value_node::get()
{
    return m_value;
}

void conf_file::float_end_value_node::set(const value_t value)
{
    m_value = value;
}

conf_file::float_end_value_node& conf_file::float_end_value_node::operator=(const float_end_value_node& other)
{
    if (this == &other)
    {
        return *this;
    }

    m_value = other.m_value;

    return *this;
}

conf_file::float_end_value_node& conf_file::float_end_value_node::operator=(float_end_value_node&& other)
{
    if (this == &other)
    {
        return *this;
    }

    m_value = other.m_value;

    return *this;
}

conf_file::float_end_value_node& conf_file::float_end_value_node::operator=(const value_t value)
{
    m_value = value;

    return *this;
}

conf_file::float_end_value_node::operator value_t() const
{
    return m_value;
}

std::ostream& conf_file::operator<<(std::ostream& out, const float_end_value_node& f)
{
    out << f.m_value;
    return out;
}

std::istream& conf_file::operator>>(std::istream& in, float_end_value_node& f)
{
    in >> f.m_value;
    return in;
}
