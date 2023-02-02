#ifndef LIBCONFIGFILE_SECTION_NODE_HPP
#define LIBCONFIGFILE_SECTION_NODE_HPP

#include "key.hpp"
#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"

#include <initializer_list>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace libconfigfile
{
    class section_node : public node
    {
        public:

            using map_t = std::unordered_map<key, node_ptr<node>>;
            using key_type = map_t::key_type;
            using mapped_type = map_t::mapped_type;
            using value_type = map_t::value_type;
            using size_type = map_t::size_type;
            using difference_type = map_t::difference_type;
            using hasher = map_t::hasher;
            using key_equal = map_t::key_equal;
            using allocator_type = map_t::allocator_type;
            using reference = map_t::reference;
            using const_reference = map_t::const_reference;
            using pointer = map_t::pointer;
            using const_pointer = map_t::const_pointer;
            using iterator = map_t::iterator;
            using const_iterator = map_t::const_iterator;
            using local_iterator = map_t::local_iterator;
            using const_local_iterator = map_t::const_local_iterator;
            using map_node_type = map_t::node_type;
            using insert_return_type = map_t::insert_return_type;

        private:

            map_t m_contents;

        public:

            section_node();
            section_node(const section_node& other);
            section_node(section_node&& other);

            virtual ~section_node() override;

        public:

            virtual actual_node_type get_actual_node_type() const override;

            virtual section_node* create_new() const override;
            virtual section_node* create_clone() const override;
            virtual libconfigfile::node_type get_node_type() const override final;

            iterator begin();
            const_iterator begin() const;
            const_iterator cbegin() const;
            iterator end();
            const_iterator end() const;
            const_iterator cend() const;

            bool empty() const;
            size_type size() const;
            size_type max_size() const;

            void clear();
            std::pair<iterator,bool> insert(const value_type& value);
            std::pair<iterator,bool> insert(value_type&& value);
            template<typename P>
                std::enable_if<std::is_constructible<value_type, P&&>::value, std::pair<iterator,bool>>
                insert(P&& value)
                {
                    return m_contents.insert(std::move(value));
                }
            iterator insert(const_iterator hint, const value_type& value);
            iterator insert(const_iterator hint, value_type&& value);
            template<typename P>
                std::enable_if<std::is_constructible<value_type, P&&>::value, iterator>
                insert(const_iterator hint, P&& value)
                {
                    return m_contents.insert(hint, std::move(value));
                }
            template<typename InputIt>
                void insert(InputIt first, InputIt last)
                {
                    m_contents.insert(first, last);
                }
            void insert(std::initializer_list<value_type> ilist);
            insert_return_type insert(map_node_type&& nh);
            iterator insert(const_iterator hint, map_node_type&& nh);
            template<typename M>
                std::pair<iterator,bool> insert_or_assign(const key_type& k, M&& obj)
                {
                    return m_contents.insert_or_assign(k, std::move(obj));
                }
            template<typename M>
                std::pair<iterator,bool> insert_or_assign(key_type&& k, M&& obj)
                {
                    return m_contents.insert_or_assign(std::move(k), std::move(obj));
                }
            template<typename M>
                iterator insert_or_assign(const_iterator hint, const key_type& k, M&& obj)
                {
                    return m_contents.insert_or_assign(hint, k, std::move(obj));
                }
            template<typename M>
                iterator insert_or_assign(const_iterator hint, key_type&& k, M&& obj)
                {
                    return m_contents.insert_or_assign(hint, std::move(k), std::move(obj));
                }
            template<typename... Args>
                std::pair<iterator,bool> emplace(Args&&... args)
                {
                    return m_contents.emplace(std::move(args...));
                }
            template<typename... Args>
                iterator emplace_hint(const_iterator hint, Args&&... args)
                {
                    return m_contents.emplace(hint, std::move(args...));
                }
            template<typename... Args>
                std::pair<iterator,bool> try_emplace(const key_type& k, Args&&... args)
                {
                    return m_contents.try_emplace(k, std::move(args...));
                }
            template<typename... Args>
                std::pair<iterator,bool> try_emplace(key_type&& k, Args&&... args)
                {
                    return m_contents.try_emplace(std::move(k), std::move(args...));
                }
            template<typename... Args>
                iterator try_emplace(const_iterator hint, const key_type& k, Args&&... args)
                {
                    return m_contents.try_emplace(hint, k, std::move(args...));
                }
            template<typename... Args>
                iterator try_emplace(const_iterator hint, key_type&& k, Args&&... args)
                {
                    return m_contents.try_emplace(hint, std::move(k), std::move(args...));
                }
            iterator erase(const_iterator pos);
            iterator erase(const_iterator first, const_iterator last);
            size_type erase(const key_type& k);
            void swap(section_node& other);
            map_node_type extract(const_iterator position);
            map_node_type extract(const key_type& k);
            void merge(section_node& source);
            void merge(section_node&& source);

            mapped_type& at(const key_type& key);
            const mapped_type& at(const key_type& key) const;
            mapped_type& operator[](const key_type& key);
            mapped_type& operator[](key_type&& key);
            size_type count(const key_type& key) const;
            template<typename K>
                auto count(const K& x) const
                -> decltype(m_contents.count(x))
                {
                    return m_contents.count(x);
                }
            iterator find(const key_type& key);
            const_iterator find(const key_type& key) const;
            template<typename K>
                auto find(const K& x)
                -> decltype(m_contents.find(x))
                {
                    return m_contents.find(x);
                }
            template<typename K>
                auto find(const K& x) const
                -> decltype(m_contents.find(x))
                {
                    return m_contents.find(x);
                }
            bool contains(const key& key) const;
            template<typename K>
                auto contains(const K& x) const
                -> decltype(m_contents.contains(x))
                {
                    return m_contents.contains(x);
                }
            std::pair<iterator,iterator> equal_range(const key_type& key);
            std::pair<const_iterator,const_iterator> equal_range(const key_type& key) const;
            template<typename K>
                auto equal_range(const K& x)
                -> decltype(m_contents.equal_range(x))
                {
                    return m_contents.equal_range(x);
                }
            template<typename K>
                auto equal_range(const K& x) const
                -> decltype(m_contents.equal_range(x))
                {
                    return m_contents.equal_range(x);
                }

            local_iterator begin(size_type n);
            const_local_iterator begin(size_type n) const;
            const_local_iterator cbegin(size_type n) const;
            local_iterator end(size_type n);
            const_local_iterator end(size_type n) const;
            const_local_iterator cend(size_type n) const;
            size_type bucket_count() const;
            size_type max_bucket_count() const;
            size_type bucket_size(size_type n) const;
            size_type bucket(const key_type& key) const;

            float load_factor() const;
            float max_load_factor() const;
            void max_load_factor(float ml);
            void rehash(size_type count);
            void reserve(size_type count);

            hasher hash_function() const;
            key_equal key_eq() const;

        public:

            section_node& operator=(const section_node& other);
            section_node& operator=(section_node&& other);

        public:

            friend void swap(section_node& lhs, section_node& rhs);
            template<typename Pred>
                friend size_type erase_if(section_node& c, Pred pred);
    };

    void swap(section_node& lhs, section_node& rhs);
    template<typename Pred>
        section_node::size_type erase_if(section_node& c, Pred pred)
        {
            using std::erase_if;
            return erase_if(c.m_contents, pred);
        }
}

#endif
