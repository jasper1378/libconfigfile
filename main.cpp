#include "include/libconfigfile.hpp"

#include <cassert>
#include <iostream>

int main() {
  using namespace libconfigfile;

  node_ptr<section_node> a{make_node_ptr<section_node>(
      std::initializer_list<section_node::value_type>{
          {"i1", make_node_ptr<integer_end_value_node>(1)},
          {"i2", make_node_ptr<integer_end_value_node>(2)},
          {"i3", make_node_ptr<integer_end_value_node>(3)}})};
  std::cerr << a << '\n';

  parser p{"/home/jasper1378/Downloads/test_file.conf"};
  node_ptr<section_node> r{p.get_result()};
  std::cerr << r << '\n';
}
