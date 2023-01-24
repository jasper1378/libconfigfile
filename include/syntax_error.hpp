#ifndef CONF_FILE_SYNTAX_ERROR_HPP
#define CONF_FILE_SYNTAX_ERROR_HPP

#include "file.hpp"

#include <exception>
#include <stdexcept>
#include <string>

namespace conf_file
{
    class syntax_error : public std::runtime_error
    {
        public:

            explicit syntax_error(const std::string& what_arg);
            explicit syntax_error(const char* what_arg);
            syntax_error(const syntax_error& other) noexcept;
            syntax_error(syntax_error&& other) noexcept;

            virtual ~syntax_error() override;

        public:

            virtual const char* what() const noexcept override;

        public:

            syntax_error& operator=(const syntax_error& other) noexcept;
            syntax_error& operator=(syntax_error&& other) noexcept;

        public:

            static syntax_error generate_formatted_error(const std::string& file_name, const size_t pos_line, const size_t pos_char, const std::string& what_arg);
            static syntax_error generate_formatted_error(const file& f, const file_pos& pos, const std::string& what_arg);
            static syntax_error generate_formatted_error(const std::string& file_name, const file_pos& pos, const std::string& what_arg);
            static syntax_error generate_formatted_error(const file& f, const size_t pos_line, const size_t pos_char, const std::string& what_arg);
    };
}

#endif
