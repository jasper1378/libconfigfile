#ifndef LIBCONFIGFILE_SEMANTIC_ERROR_HPP
#define LIBCONFIGFILE_SEMANTIC_ERROR_HPP

#include "file.hpp"

#include <exception>
#include <stdexcept>
#include <string>

namespace libconfigfile
{
    class semantic_error : public std::runtime_error
    {
        public:

            explicit semantic_error(const std::string& what_arg);
            explicit semantic_error(const char* what_arg);
            semantic_error(const semantic_error& other) noexcept;
            semantic_error(semantic_error&& other) noexcept;

            virtual ~semantic_error() override;

        public:

            virtual const char* what() const noexcept override;

        public:

            semantic_error& operator=(const semantic_error& other) noexcept;
            semantic_error& operator=(semantic_error&& other) noexcept;

        public:

            static semantic_error generate_formatted_error(const std::string& file_name, const size_t pos_line, const size_t pos_char, const std::string& what_arg);
            static semantic_error generate_formatted_error(const file& f, const file_pos& pos, const std::string& what_arg);
            static semantic_error generate_formatted_error(const std::string& file_name, const file_pos& pos, const std::string& what_arg);
            static semantic_error generate_formatted_error(const file& f, const size_t pos_line, const size_t pos_char, const std::string& what_arg);
    };
}

#endif
