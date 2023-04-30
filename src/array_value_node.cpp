#include "array_value_node.hpp"

#include "node.hpp"
#include "node_types.hpp"
#include "value_node.hpp"

#include <algorithm>
#include <initializer_list>
#include <memory>
#include <utility>
#include <vector>

libconfigfile::array_value_node::array_value_node() : m_contents{} {}

libconfigfile::array_value_node::array_value_node(size_type count)
    : m_contents{count} {}

libconfigfile::array_value_node::array_value_node(size_type count,
                                                  const value_type &value)
    : m_contents{count, value} {}

libconfigfile::array_value_node::array_value_node(
    std::initializer_list<value_type> init)
    : m_contents{init} {}

libconfigfile::array_value_node::array_value_node(const array_value_node &other)
    : m_contents{other.m_contents} {}

libconfigfile::array_value_node::array_value_node(
    array_value_node &&other) noexcept
    : m_contents{std::move(other.m_contents)} {}

libconfigfile::array_value_node::~array_value_node() {}

libconfigfile::array_value_node *
libconfigfile::array_value_node::create_new() const {
  return new array_value_node{};
}

libconfigfile::array_value_node *
libconfigfile::array_value_node::create_clone() const {
  return new array_value_node{*this};
}

libconfigfile::absolute_node_type
libconfigfile::array_value_node::get_absolute_node_type() const {
  return absolute_node_type::ARRAY;
}

libconfigfile::value_node_type
libconfigfile::array_value_node::get_value_node_type() const {
  return value_node_type::ARRAY;
}

bool libconfigfile::array_value_node::polymorphic_value_compare(
    const node *other) const {
  if ((other->get_absolute_node_type()) == (absolute_node_type::ARRAY)) {
    return ((*(dynamic_cast<const array_value_node *>(other))) == (*this));
  } else {
    return false;
  }
}

void libconfigfile::array_value_node::assign(size_type count,
                                             const value_type &value) {
  m_contents.assign(count, value);
}

void libconfigfile::array_value_node::assign(
    std::initializer_list<value_type> ilist) {
  m_contents.assign(ilist);
}

libconfigfile::array_value_node::allocator_type
libconfigfile::array_value_node::get_allocator() const {
  return m_contents.get_allocator();
}

libconfigfile::array_value_node::reference
libconfigfile::array_value_node::at(size_type pos) {
  return m_contents.at(pos);
}

libconfigfile::array_value_node::const_reference
libconfigfile::array_value_node::at(size_type pos) const {
  return m_contents.at(pos);
}

libconfigfile::array_value_node::reference
libconfigfile::array_value_node::operator[](size_type pos) {
  return m_contents[pos];
}

libconfigfile::array_value_node::const_reference
libconfigfile::array_value_node::operator[](size_type pos) const {
  return m_contents[pos];
}

libconfigfile::array_value_node::reference
libconfigfile::array_value_node::front() {
  return m_contents.front();
}

libconfigfile::array_value_node::const_reference
libconfigfile::array_value_node::front() const {
  return m_contents.front();
}

libconfigfile::array_value_node::reference
libconfigfile::array_value_node::back() {
  return m_contents.back();
}

libconfigfile::array_value_node::const_reference
libconfigfile::array_value_node::back() const {
  return m_contents.back();
}

libconfigfile::array_value_node::value_type *
libconfigfile::array_value_node::data() {
  return m_contents.data();
}

const libconfigfile::array_value_node::value_type *
libconfigfile::array_value_node::data() const {
  return m_contents.data();
}

libconfigfile::array_value_node::iterator
libconfigfile::array_value_node::begin() {
  return m_contents.begin();
}

libconfigfile::array_value_node::const_iterator
libconfigfile::array_value_node::begin() const {
  return m_contents.begin();
}

libconfigfile::array_value_node::const_iterator
libconfigfile::array_value_node::cbegin() const {
  return m_contents.cbegin();
}

libconfigfile::array_value_node::iterator
libconfigfile::array_value_node::end() {
  return m_contents.end();
}

libconfigfile::array_value_node::const_iterator
libconfigfile::array_value_node::end() const {
  return m_contents.end();
}

libconfigfile::array_value_node::const_iterator
libconfigfile::array_value_node::cend() const {
  return m_contents.cend();
}

libconfigfile::array_value_node::reverse_iterator
libconfigfile::array_value_node::rbegin() {
  return m_contents.rbegin();
}

libconfigfile::array_value_node::const_reverse_iterator
libconfigfile::array_value_node::rbegin() const {
  return m_contents.rbegin();
}

libconfigfile::array_value_node::const_reverse_iterator
libconfigfile::array_value_node::crbegin() const {
  return m_contents.crbegin();
}

libconfigfile::array_value_node::reverse_iterator
libconfigfile::array_value_node::rend() {
  return m_contents.rend();
}

libconfigfile::array_value_node::const_reverse_iterator
libconfigfile::array_value_node::rend() const {
  return m_contents.rend();
}

libconfigfile::array_value_node::const_reverse_iterator
libconfigfile::array_value_node::crend() const {
  return m_contents.crend();
}

bool libconfigfile::array_value_node::empty() const {
  return m_contents.empty();
}

libconfigfile::array_value_node::size_type
libconfigfile::array_value_node::size() const {
  return m_contents.size();
}

libconfigfile::array_value_node::size_type
libconfigfile::array_value_node::max_size() const {
  return m_contents.max_size();
}

void libconfigfile::array_value_node::reserve(size_type new_cap) {
  m_contents.reserve(new_cap);
}

libconfigfile::array_value_node::size_type
libconfigfile::array_value_node::capacity() const {
  return m_contents.capacity();
}

void libconfigfile::array_value_node::shrink_to_fit() {
  m_contents.shrink_to_fit();
}

void libconfigfile::array_value_node::clear() { m_contents.clear(); }

libconfigfile::array_value_node::iterator
libconfigfile::array_value_node::insert(const_iterator pos,
                                        const value_type &value) {
  return m_contents.insert(pos, value);
}

libconfigfile::array_value_node::iterator
libconfigfile::array_value_node::insert(const_iterator pos,
                                        value_type &&value) {
  return m_contents.insert(pos, std::move(value));
}

libconfigfile::array_value_node::iterator
libconfigfile::array_value_node::insert(const_iterator pos, size_type count,
                                        const value_type &value) {
  return m_contents.insert(pos, count, value);
}

libconfigfile::array_value_node::iterator
libconfigfile::array_value_node::insert(
    const_iterator pos, std::initializer_list<value_type> ilist) {
  return m_contents.insert(pos, ilist);
}

libconfigfile::array_value_node::iterator
libconfigfile::array_value_node::erase(const_iterator pos) {
  return m_contents.erase(pos);
}

libconfigfile::array_value_node::iterator
libconfigfile::array_value_node::erase(const_iterator first,
                                       const_iterator last) {
  return m_contents.erase(first, last);
}

void libconfigfile::array_value_node::push_back(const value_type &value) {
  m_contents.push_back(value);
}

void libconfigfile::array_value_node::push_back(value_type &&value) {
  m_contents.push_back(std::move(value));
}

void libconfigfile::array_value_node::pop_back() { m_contents.pop_back(); }

void libconfigfile::array_value_node::resize(size_type count) {
  m_contents.resize(count);
}

void libconfigfile::array_value_node::resize(size_type count,
                                             const value_type &value) {
  m_contents.resize(count, value);
}

void libconfigfile::array_value_node::swap(array_value_node &other) {
  m_contents.swap(other.m_contents);
}

libconfigfile::array_value_node &
libconfigfile::array_value_node::operator=(const array_value_node &other) {
  m_contents = other.m_contents;

  return *this;
}

libconfigfile::array_value_node &
libconfigfile::array_value_node::operator=(array_value_node &&other) noexcept(
    std::allocator_traits<
        allocator_type>::propagate_on_container_move_assignment::value ||
    std::allocator_traits<allocator_type>::is_always_equal::value) {
  m_contents = std::move(other.m_contents);

  return *this;
}

bool libconfigfile::operator==(const array_value_node &lhs,
                               const array_value_node &rhs) {
  return (lhs.m_contents == rhs.m_contents);
}

bool libconfigfile::operator!=(const array_value_node &lhs,
                               const array_value_node &rhs) {
  return (lhs.m_contents == rhs.m_contents);
}

// bool libconfigfile::operator<(const array_value_node &lhs,
//                               const array_value_node &rhs) {
//   return lhs.m_contents < rhs.m_contents;
// }
//
// bool libconfigfile::operator<=(const array_value_node &lhs,
//                                const array_value_node &rhs) {
//   return lhs.m_contents <= rhs.m_contents;
// }
//
// bool libconfigfile::operator>(const array_value_node &lhs,
//                               const array_value_node &rhs) {
//   return lhs.m_contents > rhs.m_contents;
// }
//
// bool libconfigfile::operator>=(const array_value_node &lhs,
//                                const array_value_node &rhs) {
//   return lhs.m_contents >= rhs.m_contents;
// }
//
// auto libconfigfile::operator<=>(const array_value_node &lhs,
//                                 const array_value_node &rhs)
//     -> decltype(lhs.m_contents <=> rhs.m_contents) {
//   return lhs.m_contents <=> rhs.m_contents;
// }

void libconfigfile::swap(array_value_node &lhs, array_value_node &rhs) {
  using std::swap;
  swap(lhs.m_contents, rhs.m_contents);
}
