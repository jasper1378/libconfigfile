#include "config.hpp"

#include "node.hpp"
#include "node_ptr.hpp"

#include <string>
#include <unordered_map>
#include <utility>

libconfigfile::config::config() : m_values{} {}

libconfigfile::config::config(const config &other) : m_values{other.m_values} {}

libconfigfile::config::config(config &&other)
    : m_values{std::move(other.m_values)} {}

libconfigfile::config::~config() {}

libconfigfile::config &libconfigfile::config::operator=(const config &other) {
  if (this == &other) {
    return *this;
  }

  m_values = other.m_values;

  return *this;
}

libconfigfile::config &libconfigfile::config::operator=(config &&other) {
  if (this == &other) {
    return *this;
  }

  m_values = std::move(other.m_values);

  return *this;
}
