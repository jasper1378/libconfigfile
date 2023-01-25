#ifndef LIBCONFIGFILE_CONFIG_HPP
#define LIBCONFIGFILE_CONFIG_HPP

#include "array_value_node.hpp"
#include "end_value_node.hpp"
#include "file.hpp"
#include "float_end_value_node.hpp"
#include "integer_end_value_node.hpp"
#include "node.hpp"
#include "node_ptr.hpp"
#include "node_types.hpp"
#include "section_node.hpp"
#include "semantic_error.hpp"
#include "string_end_value_node.hpp"
#include "syntax_error.hpp"
#include "value_node.hpp"

#include <string>
#include <unordered_map>

namespace libconfigfile
{
    class config
    {
        private:

            file m_raw_file_contents;
            std::unordered_map<std::string, node_ptr<node>> m_values;

        private:

            static constexpr std::string m_k_whitespace_chars{ " \t" };;
            static constexpr char m_k_newline{ '\n' };

            static const char m_k_comment_leader{ '/' };
            static constexpr std::string m_k_single_line_comment{ "//" };
            static constexpr std::string m_k_multi_line_comment_start{ "/*" };
            static constexpr std::string m_k_multi_line_comment_end{ "*/" };

            static constexpr char m_k_directive_leader{ '#' };

        public:

            config();
            config(const std::string& file_name);
            config(const config& other);
            config(config&& other);

            ~config();

        public:

            config& operator=(const config& other);
            config& operator=(config&& other);

        private:

            void parse_file();
            void parse_directive(file_pos& cur_pos);
            void parse_include_directive(file_pos& cur_pos, const std::string& args);
            //void validate_file(); //TODO

            bool is_pos_located_on_occurence_of(const file_pos& pos, const std::string& str);
            std::string get_substr_between_indices(const std::string& str, const std::string::size_type start, const std::string::size_type end);
    };
}

#endif
