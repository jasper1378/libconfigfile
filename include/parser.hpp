#ifndef LIBCONFIGFILE_PARSER_HPP
#define LIBCONFIGFILE_PARSER_HPP

#include "file.hpp"
#include "node.hpp"
#include "node_ptr.hpp"

#include <string>

namespace libconfigfile {
class parser {
private:
  file m_file_contents;
  file_pos m_cur_pos;

private:
  static constexpr std::string m_k_whitespace_chars{" \t"};
  static constexpr char m_k_newline{'\n'};

  static constexpr char m_k_comment_script{'#'};
  static constexpr std::string m_k_comment_cpp{"//"};
  static constexpr std::string m_k_comment_c_start{"/*"};
  static constexpr std::string m_k_comment_c_end{"*/"};

  static constexpr char m_k_directive_leader{'@'};

public:
  parser();
  parser(const std::string &file_name);
  parser(const parser &other);
  parser(parser &&other) /*TODO: noexcept()*/;

  ~parser();

public:
  // config parse() or config get_result() or etc.; // TODO

public:
  parser &operator=(const parser &other);
  parser &operator=(parser &&other) /*TODO: noexcept()*/;

private:
  void parse_file();

  void parse_directive();
  void parse_include_directive(const std::string &args);

  bool is_pos_located_on_occurence_of(const file_pos &pos,
                                      const std::string &str);
  std::string get_substr_between_indices(const std::string &str,
                                         const std::string::size_type start,
                                         const std::string::size_type end);
};
} // namespace libconfigfile

#endif
