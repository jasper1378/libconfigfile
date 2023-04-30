#ifndef LIBCONFIGFILE_NODE_CONCEPTS_HPP
#define LIBCONFIGFILE_NODE_CONCEPTS_HPP

#include "node.hpp"

#include <concepts>

namespace libconfigfile {

template <typename node_t>
concept is_node_type = std::derived_from<node_t, node>;

} // namespace libconfigfile

#endif
