#ifndef LIBCONFIGFILE_NODE_TYPES_HPP
#define LIBCONFIGFILE_NODE_TYPES_HPP

namespace libconfigfile {
enum class node_type {
  VALUE,
  SECTION,
  MAX,
};

enum class value_node_type {
  END_VALUE,
  ARRAY,
  MAX,
};

enum class end_value_node_type {
  INTEGER,
  FLOAT,
  STRING,
  MAX,
};
} // namespace libconfigfile

#endif
