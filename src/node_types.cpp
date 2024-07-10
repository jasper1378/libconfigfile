#include "node_types.hpp"

#include <string>

std::string libconfigfile::node_type_to_str(const node_type type) {
  switch (type) {
  case ::libconfigfile::node_type::String: {
    return "string";
  } break;
  case ::libconfigfile::node_type::Integer: {
    return "integer";
  } break;
  case ::libconfigfile::node_type::Float: {
    return "float";
  } break;
  case ::libconfigfile::node_type::Array: {
    return "array";
  } break;
  case ::libconfigfile::node_type::Map: {
    return "map";
  } break;
  default: {
    return "null";
  } break;
  }
}
