#include "section_node.hpp"

#include "node.hpp"
#include "node_types.hpp"

#include <initializer_list>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <utility>

libconfigfile::section_node::section_node() : m_contents{} {}

libconfigfile::section_node::section_node(size_type bucket_count)
    : m_contents{bucket_count} {}

libconfigfile::section_node::section_node(
    std::initializer_list<value_type> init)
    : m_contents{init} {}

libconfigfile::section_node::section_node(
    std::initializer_list<value_type> init, size_type bucket_count)
    : m_contents{init, bucket_count} {}

libconfigfile::section_node::section_node(const section_node &other)
    : m_contents{other.m_contents} {}

libconfigfile::section_node::section_node(section_node &&other)
    : m_contents{std::move(other.m_contents)} {}

libconfigfile::section_node::~section_node() {}

libconfigfile::actual_node_type
libconfigfile::section_node::get_actual_node_type() const {
  return actual_node_type::SECTION_NODE;
}

libconfigfile::section_node *libconfigfile::section_node::create_new() const {
  return new section_node{};
}

libconfigfile::section_node *libconfigfile::section_node::create_clone() const {
  return new section_node{*this};
}

libconfigfile::node_type libconfigfile::section_node::get_node_type() const {
  return libconfigfile::node_type::SECTION;
}

libconfigfile::section_node::allocator_type
libconfigfile::section_node::get_allocator() const {
  return m_contents.get_allocator();
}

libconfigfile::section_node::iterator libconfigfile::section_node::begin() {
  return m_contents.begin();
}

libconfigfile::section_node::const_iterator
libconfigfile::section_node::begin() const {
  return m_contents.begin();
}

libconfigfile::section_node::const_iterator
libconfigfile::section_node::cbegin() const {
  return m_contents.cbegin();
}

libconfigfile::section_node::iterator libconfigfile::section_node::end() {
  return m_contents.end();
}

libconfigfile::section_node::const_iterator
libconfigfile::section_node::end() const {
  return m_contents.end();
}

libconfigfile::section_node::const_iterator
libconfigfile::section_node::cend() const {
  return m_contents.cend();
}

bool libconfigfile::section_node::empty() const { return m_contents.empty(); }

libconfigfile::section_node::size_type
libconfigfile::section_node::size() const {
  return m_contents.size();
}

libconfigfile::section_node::size_type
libconfigfile::section_node::max_size() const {
  return m_contents.max_size();
}

void libconfigfile::section_node::clear() { m_contents.clear(); }

std::pair<libconfigfile::section_node::iterator, bool>
libconfigfile::section_node::insert(const value_type &value) {
  return m_contents.insert(value);
}

std::pair<libconfigfile::section_node::iterator, bool>
libconfigfile::section_node::insert(value_type &&value) {
  return m_contents.insert(std::move(value));
}

libconfigfile::section_node::iterator
libconfigfile::section_node::insert(const_iterator hint,
                                    const value_type &value) {
  return m_contents.insert(hint, value);
}

libconfigfile::section_node::iterator
libconfigfile::section_node::insert(const_iterator hint, value_type &&value) {
  return m_contents.insert(hint, std::move(value));
}

void libconfigfile::section_node::insert(
    std::initializer_list<value_type> ilist) {
  m_contents.insert(ilist);
}

libconfigfile::section_node::insert_return_type
libconfigfile::section_node::insert(map_node_type &&nh) {
  return m_contents.insert(std::move(nh));
}

libconfigfile::section_node::iterator
libconfigfile::section_node::insert(const_iterator hint, map_node_type &&nh) {
  return m_contents.insert(hint, std::move(nh));
}

libconfigfile::section_node::iterator
libconfigfile::section_node::erase(const_iterator pos) {
  return m_contents.erase(pos);
}

libconfigfile::section_node::iterator
libconfigfile::section_node::erase(const_iterator first, const_iterator last) {
  return m_contents.erase(first, last);
}

libconfigfile::section_node::size_type
libconfigfile::section_node::erase(const key_type &k) {
  return m_contents.erase(k);
}

void libconfigfile::section_node::swap(section_node &other) {
  m_contents.swap(other.m_contents);
}

libconfigfile::section_node::map_node_type
libconfigfile::section_node::extract(const_iterator position) {
  return m_contents.extract(position);
}

libconfigfile::section_node::map_node_type
libconfigfile::section_node::extract(const key_type &k) {
  return m_contents.extract(k);
}

void libconfigfile::section_node::merge(section_node &other) {
  m_contents.merge(other.m_contents);
}

void libconfigfile::section_node::merge(section_node &&other) {
  m_contents.merge(std::move(other.m_contents));
}

libconfigfile::section_node::mapped_type &
libconfigfile::section_node::at(const key_type &key) {
  return m_contents.at(key);
}

const libconfigfile::section_node::mapped_type &
libconfigfile::section_node::at(const key_type &key) const {
  return m_contents.at(key);
}

libconfigfile::section_node::mapped_type &
libconfigfile::section_node::operator[](const key_type &key) {
  return m_contents[key];
}

libconfigfile::section_node::mapped_type &
libconfigfile::section_node::operator[](key_type &&key) {
  return m_contents[std::move(key)];
}

libconfigfile::section_node::size_type
libconfigfile::section_node::count(const key_type &key) const {
  return m_contents.count(key);
}

libconfigfile::section_node::iterator
libconfigfile::section_node::find(const key_type &key) {
  return m_contents.find(key);
}

libconfigfile::section_node::const_iterator
libconfigfile::section_node::find(const key_type &key) const {
  return m_contents.find(key);
}

std::pair<libconfigfile::section_node::iterator,
          libconfigfile::section_node::iterator>
libconfigfile::section_node::equal_range(const key_type &key) {
  return m_contents.equal_range(key);
}

std::pair<libconfigfile::section_node::const_iterator,
          libconfigfile::section_node::const_iterator>
libconfigfile::section_node::equal_range(const key_type &key) const {
  return m_contents.equal_range(key);
}

libconfigfile::section_node::local_iterator
libconfigfile::section_node::begin(size_type n) {
  return m_contents.begin(n);
}

libconfigfile::section_node::const_local_iterator
libconfigfile::section_node::begin(size_type n) const {
  return m_contents.begin(n);
}

libconfigfile::section_node::const_local_iterator
libconfigfile::section_node::cbegin(size_type n) const {
  return m_contents.cbegin(n);
}

libconfigfile::section_node::local_iterator
libconfigfile::section_node::end(size_type n) {
  return m_contents.end(n);
}

libconfigfile::section_node::const_local_iterator
libconfigfile::section_node::end(size_type n) const {
  return m_contents.end(n);
}

libconfigfile::section_node::const_local_iterator
libconfigfile::section_node::cend(size_type n) const {
  return m_contents.cend(n);
}

libconfigfile::section_node::size_type
libconfigfile::section_node::bucket_count() const {
  return m_contents.bucket_count();
}

libconfigfile::section_node::size_type
libconfigfile::section_node::max_bucket_count() const {
  return m_contents.max_bucket_count();
}

libconfigfile::section_node::size_type
libconfigfile::section_node::bucket_size(size_type n) const {
  return m_contents.bucket_size(n);
}

libconfigfile::section_node::size_type
libconfigfile::section_node::bucket(const key_type &key) const {
  return m_contents.bucket(key);
}

float libconfigfile::section_node::load_factor() const {
  return m_contents.load_factor();
}

float libconfigfile::section_node::max_load_factor() const {
  return m_contents.max_load_factor();
}

void libconfigfile::section_node::max_load_factor(float ml) {
  m_contents.max_load_factor(ml);
}

void libconfigfile::section_node::rehash(size_type count) {
  m_contents.rehash(count);
}

void libconfigfile::section_node::reserve(size_type count) {
  m_contents.reserve(count);
}

libconfigfile::section_node::hasher
libconfigfile::section_node::hash_function() const {
  return m_contents.hash_function();
}

libconfigfile::section_node::key_equal
libconfigfile::section_node::key_eq() const {
  return m_contents.key_eq();
}

libconfigfile::section_node &
libconfigfile::section_node::operator=(const section_node &other) {
  if (this == &other) {
    return *this;
  }

  m_contents = other.m_contents;

  return *this;
}

libconfigfile::section_node &
libconfigfile::section_node::operator=(section_node &&other) noexcept(
    std::is_nothrow_assignable_v<decltype(m_contents), decltype(m_contents)>) {
  if (this == &other) {
    return *this;
  }

  m_contents = std::move(other.m_contents);

  return *this;
}

bool libconfigfile::operator==(const section_node &lhs,
                               const section_node &rhs) {
  return lhs.m_contents == rhs.m_contents;
}

bool libconfigfile::operator!=(const section_node &lhs,
                               const section_node &rhs) {
  return lhs.m_contents != rhs.m_contents;
}

void libconfigfile::swap(section_node &lhs, section_node &rhs) {
  using std::swap;
  swap(lhs.m_contents, rhs.m_contents);
}
