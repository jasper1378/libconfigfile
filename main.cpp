#include "include/libconfigfile.hpp"

#include <cassert>
#include <iostream>

int main() {
  using namespace libconfigfile;

  node_ptr<section_node> r{
      parser::parse("/home/jasper1378/Downloads/test_file.conf")};
  std::cerr << r << '\n';
}
