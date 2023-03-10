#ifndef LIBCONFIGFILE_NODE_HPP
#define LIBCONFIGFILE_NODE_HPP

#include "node_types.hpp"

#include <cstddef>

namespace libconfigfile {
class node {
public:
  virtual ~node();

public:
  virtual node *create_new() const = 0;
  virtual node *create_clone() const = 0;
  virtual node_type get_node_type() const = 0;
};
} // namespace libconfigfile

#endif
