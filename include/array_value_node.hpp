#ifndef LIBCONFIGFILE_ARRAY_VALUE_NODE_HPP
#define LIBCONFIGFILE_ARRAY_VALUE_NODE_HPP

#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"
#include "value_node.hpp"

#include <initializer_list>
#include <type_traits>
#include <utility>
#include <vector>

namespace libconfigfile {
class array_value_node : public value_node {
public:
  using vector_t = std::vector<node_ptr<value_node>>;
  using value_type = vector_t::value_type;
  using allocator_type = vector_t::allocator_type;
  using size_type = vector_t::size_type;
  using difference_type = vector_t::difference_type;
  using reference = vector_t::reference;
  using const_reference = vector_t::const_reference;
  using pointer = vector_t::pointer;
  using const_pointer = vector_t::const_pointer;
  using iterator = vector_t::iterator;
  using const_iterator = vector_t::const_iterator;
  using reverse_iterator = vector_t::reverse_iterator;
  using const_reverse_iterator = vector_t::const_reverse_iterator;

private:
  vector_t m_contents;

public:
  array_value_node();
  array_value_node(const array_value_node &other);
  array_value_node(array_value_node &&other);

  virtual ~array_value_node() override;

public:
  virtual actual_node_type get_actual_node_type() const override;

  virtual array_value_node *create_new() const override;
  virtual array_value_node *create_clone() const override;
  virtual value_node_type get_value_node_type() const override final;

  reference at(size_type pos);
  const_reference at(size_type pos) const;
  reference operator[](size_type pos);
  const_reference operator[](size_type pos) const;
  reference front();
  const_reference front() const;
  reference back();
  const_reference back() const;
  value_type *data();
  const value_type *data() const;

  iterator begin();
  const_iterator begin() const;
  const_iterator cbegin() const;
  iterator end();
  const_iterator end() const;
  const_iterator cend() const;
  reverse_iterator rbegin();
  const_reverse_iterator rbegin() const;
  const_reverse_iterator crbegin() const;
  reverse_iterator rend();
  const_reverse_iterator rend() const;
  const_reverse_iterator crend() const;

  bool empty() const;
  size_type size() const;
  size_type max_size() const;
  void reserve(size_type new_cap);
  size_type capacity() const;
  void shrink_to_fit();

  void clear();
  iterator insert(const_iterator pos, const value_type &value);
  iterator insert(const_iterator pos, value_type &&value);
  iterator insert(const_iterator pos, size_type count, const value_type &value);
  template <typename InputIt>
  iterator insert(const_iterator pos, InputIt first, InputIt last) {
    return m_contents.insert(pos, first, last);
  }
  iterator insert(const_iterator pos, std::initializer_list<value_type> ilist);
  template <typename... Args>
  iterator emplace(const_iterator pos, Args &&...args) {
    return m_contents.emplace(pos, std::move(args...));
  }
  iterator erase(const_iterator pos);
  iterator erase(const_iterator first, const_iterator last);
  void push_back(const value_type &value);
  void push_back(value_type &&value);
  template <typename... Args> reference emplace_back(Args &&...args) {
    return m_contents.emplace_back(std::move(args...));
  }
  void pop_back();
  void resize(size_type count);
  void resize(size_type count, const value_type &value);
  void swap(array_value_node &other);

public:
  array_value_node &operator=(const array_value_node &other);
  array_value_node &operator=(array_value_node &&other);

public:
  friend bool operator==(const array_value_node &lhs,
                         const array_value_node &rhs);
  friend auto operator<=>(const array_value_node &lhs,
                          const array_value_node &rhs)
      -> decltype(lhs.m_contents <=> rhs.m_contents);
  friend void swap(array_value_node &lhs, array_value_node &rhs);
  template <typename U>
  friend array_value_node::size_type erase(array_value_node &c, const U &value);
  template <typename Pred>
  friend array_value_node::size_type erase_if(array_value_node &c, Pred pred);
};

bool operator==(const array_value_node &lhs, const array_value_node &rhs);
auto operator<=>(const array_value_node &lhs, const array_value_node &rhs)
    -> decltype(lhs.m_contents <=> rhs.m_contents);
void swap(array_value_node &lhs, array_value_node &rhs);
template <typename U>
array_value_node::size_type erase(array_value_node &c, const U &value) {
  using std::erase;
  return erase(c.m_contents, value);
}
template <typename Pred>
array_value_node::size_type erase_if(array_value_node &c, Pred pred) {
  using std::erase_if;
  return erase_if(c, pred);
}
} // namespace libconfigfile

#endif
