#include "include/libconfigfile.hpp"

#include <cassert>
#include <iostream>

int main() {
  using namespace libconfigfile;

  parser p{"/home/jasper1378/Downloads/test_file.conf"};
  node_ptr<section_node> r{p.get_result()};
  std::cerr << r << '\n';
}
