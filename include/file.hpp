#ifndef LIBCONFIGFILE_FILE_HPP
#define LIBCONFIGFILE_FILE_HPP

#include <string>
#include <type_traits>
#include <vector>

namespace libconfigfile {
class file_pos;

class file {
private:
  std::string m_file_path;
  std::vector<std::string> m_file_contents;

private:
  static constexpr char m_k_newline{'\n'};

public:
  file();
  file(const std::string &file_path, bool insert_newlines = true);
  file(std::string &&file_path, bool insert_newlines = true);
  file(const file &other);
  file(file &&other) noexcept;

  ~file();

public:
  bool is_paired(const file_pos &pos) const;

  std::string get_file_path() const;

  file_pos create_file_pos() const;
  file_pos create_file_pos(const file_pos &start_pos) const;

  const char &get_char(const file_pos &pos) const;
  const std::string &get_line(const file_pos &pos) const;
  const std::vector<std::string> &get_array() const;

  std::vector<std::string> &get_underlying();

public:
  file &operator=(const file &other);
  file &operator=(file &&other) noexcept(
      (std::is_nothrow_assignable_v<
          decltype(m_file_path),
          decltype(m_file_path)>)&&(std::
                                        is_nothrow_assignable_v<
                                            decltype(m_file_contents),
                                            decltype(m_file_contents)>));

  const char &operator[](const file_pos &pos) const;

private:
  std::vector<std::string> read_file(const std::string &file_path,
                                     const bool insert_newlines = false);
};

class file_pos {
private:
  const file *m_file;
  size_t m_line;
  size_t m_char;
  bool m_bof;
  bool m_eof;

public:
  file_pos(const file *file_in_which_to_move);
  file_pos(const file *file_in_which_to_move, const file_pos &start_pos);
  file_pos(const file_pos &other);
  file_pos(file_pos &&other);

public:
  ~file_pos();

public:
  bool is_paired(const file &f) const;
  const file *get_paired_file() const;

  bool is_bof() const;
  bool is_eof() const;
  size_t get_line() const;
  size_t get_char() const;

  void set_line(const size_t line_val);
  void set_char(const size_t char_val);

  void goto_next_line(size_t lines_to_move = 1);
  void goto_prev_line(size_t lines_to_move = 1);
  void goto_next_char(size_t chars_to_move = 1);
  void goto_prev_char(size_t chars_to_move = 1);

  void goto_find_start(const std::string &to_find);
  void goto_find_end(const std::string &to_find);
  void goto_rfind_start(const std::string &to_find);
  void goto_rfind_end(const std::string &to_find);

  void goto_find_first_of(const std::string &to_find);
  void goto_find_first_of(const std::vector<char> &to_find);
  void goto_find_first_not_of(const std::string &to_find);
  void goto_find_first_not_of(const std::vector<char> &to_find);
  void goto_find_last_of(const std::string &to_find);
  void goto_find_last_of(const std::vector<char> &to_find);
  void goto_find_last_not_of(const std::string &to_find);
  void goto_find_last_not_of(const std::vector<char> &to_find);

  void goto_end_of_whitespace(const std::string &whitespace_chars = " \t");
  void goto_end_of_whitespace(const std::vector<char> &whitespace_chars = {
                                  ' ', '\t'});
  void goto_start_of_whitespace(const std::string &whitespace_chars = " \t");
  void goto_start_of_whitespace(const std::vector<char> &whitespace_chars = {
                                    ' ', '\t'});

  bool is_located_on_occurence_of(const std::string &str) const;
  bool is_located_on_occurence_of(const char ch) const;

public:
  file_pos &operator=(const file_pos &other);
  file_pos &operator=(file_pos &&other);

  bool operator==(const file_pos &other) const;
  bool operator!=(const file_pos &other) const;

  file_pos &operator++();
  file_pos operator++(int);
  file_pos &operator--();
  file_pos operator--(int);

  file_pos &operator+=(const size_t chars_to_move);
  file_pos &operator-=(const size_t chars_to_move);

private:
  void set_bof();
  void set_eof();

public:
  friend file_pos operator+(const file_pos pos, const size_t chars_to_move);
  friend file_pos operator+(const size_t chars_to_move, const file_pos pos);
  friend file_pos operator-(const file_pos pos, const size_t chars_to_move);
  friend file_pos operator-(const size_t chars_to_move, const file_pos pos);
};

file_pos operator+(file_pos pos, const size_t chars_to_move);
file_pos operator+(const size_t chars_to_move, file_pos pos);
file_pos operator-(file_pos pos, const size_t chars_to_move);
file_pos operator-(const size_t chars_to_move, file_pos pos);
} // namespace libconfigfile

#endif
