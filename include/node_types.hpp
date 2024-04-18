#ifndef LIBCONFIGFILE_NODE_TYPES_HPP
#define LIBCONFIGFILE_NODE_TYPES_HPP

#include <string>

namespace libconfigfile {
enum class node_type {
  // Look, I know the case of the enumerators is nasty and inconsistent with the
  // rest of my stuff. But I can't use 'float' as an identifier!
  // Aaarrrggghhh!!!
  String,
  Integer,
  Float,
  Array,
  Map,
  Null,
};

std::string node_type_to_str(const node_type type);

} // namespace libconfigfile

#endif
