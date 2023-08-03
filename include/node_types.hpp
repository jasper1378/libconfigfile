#ifndef LIBCONFIGFILE_NODE_TYPES_HPP
#define LIBCONFIGFILE_NODE_TYPES_HPP

namespace libconfigfile {
enum class node_type {
  MAP,
  ARRAY,
  STRING,
  FLOAT,
  INTEGER,
  MAX,
};
} // namespace libconfigfile

#endif
