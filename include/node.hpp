#ifndef LIBCONFIGFILE_NODE_HPP
#define LIBCONFIGFILE_NODE_HPP

#include "node_types.hpp"

#include <iostream>

namespace libconfigfile {
class node {

public:
  virtual ~node();

public:
  virtual node *create_new() const = 0;
  virtual node *create_clone() const = 0;
  virtual node_type get_node_type() const = 0;
  virtual bool polymorphic_value_compare(const node *other) const = 0;
  virtual std::ostream &print(std::ostream &out,
                              const int indent_level = 0) const = 0;
};
} // namespace libconfigfile

#endif
