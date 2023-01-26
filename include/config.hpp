#ifndef LIBCONFIGFILE_CONFIG_HPP
#define LIBCONFIGFILE_CONFIG_HPP

#include "node.hpp"
#include "node_ptr.hpp"

#include <string>
#include <unordered_map>

namespace libconfigfile
{
    //TODO
    class config
    {
        private:

            std::unordered_map<std::string, node_ptr<node>> m_values;

        public:

            config();
            config(const config& other);
            config(config&& other);

            ~config();

        public:

            //TODO
            //void validate_file(); //TODO

        public:

            config& operator=(const config& other);
            config& operator=(config&& other);

        public:

            friend class parser;
    };
}

#endif
