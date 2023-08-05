#ifndef LIBCONFIGFILE_NODE_TYPES_HPP
#define LIBCONFIGFILE_NODE_TYPES_HPP

namespace libconfigfile {
enum class node_type {
  STRING,
  INTEGER,
  FLOAT,
  ARRAY,
  MAP,
  NULLL,
};
} // namespace libconfigfile

#endif
